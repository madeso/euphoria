#include "eu/io/mesh.h"

#include "eu/core/geom.builder.h"

#include "eu/io/file.h"

#include "cgltf.h"

namespace eu::io
{

struct GltfFile
{
    cgltf_data* data = nullptr;

    void clear()
    {
        if (data == nullptr)
        {
            return;
        }

        cgltf_free(data);
        data = nullptr;
    }

    bool load_gltf_file(const std::string& path)
    {
        cgltf_options options = {};
        data = nullptr;

        // const auto bytes = bytes_from_file(path);

        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
        // cgltf_result result = cgltf_parse(&options, bytes.data(), bytes.size(), &data);
        if (result != cgltf_result_success)
        {
            clear();
            LOG_ERR("Could not parse file: {}", path);
            return false;
        }

        result = cgltf_load_buffers(&options, data, path.c_str());
        if (result != cgltf_result_success)
        {
            clear();
            LOG_ERR("Could not load buffers for: {}", path);
            return false;
        }

        result = cgltf_validate(data);
        if (result != cgltf_result_success)
        {
            clear();
            LOG_ERR("GLTF file validation FAILED: {}", path);
            return false;
        }
        return true;
    }
};


std::optional<std::size_t> find_node_index(
    cgltf_node* target, cgltf_node* allNodes, std::size_t numNodes
)
{
    if (target == 0)
    {
        return std::nullopt;
    }
    for (std::size_t i = 0; i < numNodes; ++i)
    {
        if (target == &allNodes[i])
        {
            return i;
        }
    }
    return std::nullopt;
}

std::vector<float> scalar_values_from_accessor(
    std::size_t component_count, const cgltf_accessor& accessor
)
{
    std::vector<float> outScalars;
    outScalars.resize(accessor.count * component_count);

    for (cgltf_size i = 0; i < accessor.count; ++i)
    {
        cgltf_accessor_read_float(&accessor, i, &outScalars[i * component_count], component_count);
    }
    return outScalars;
}

void extract_mesh_from_attribute(
    core::geom::Builder& outMesh,
    cgltf_attribute& attribute,
    cgltf_skin* skin,
    cgltf_node* nodes,
    unsigned int nodeCount
)
{
    cgltf_attribute_type attribType = attribute.type;
    cgltf_accessor& accessor = *attribute.data;

    unsigned int componentCount = 0;
    if (accessor.type == cgltf_type_vec2)
    {
        componentCount = 2;
    }
    else if (accessor.type == cgltf_type_vec3)
    {
        componentCount = 3;
    }
    else if (accessor.type == cgltf_type_vec4)
    {
        componentCount = 4;
    }
    const auto values = scalar_values_from_accessor(componentCount, accessor);
    const auto acessorCount = accessor.count;

    for (std::size_t i = 0; i < acessorCount; ++i)
    {
        const auto index = i * componentCount;
        switch (attribType)
        {
        case cgltf_attribute_type_position:
            outMesh.add_position(v3(values[index + 0], values[index + 1], values[index + 2])
            );
            break;
        case cgltf_attribute_type_texcoord:
            // note: eu 2d coordinate system is bottom left going up, gltf is top left going down, `1-y` fixes this
            outMesh.add_text_coord(v2(values[index + 0], 1-values[index + 1]));
            break;
        case cgltf_attribute_type_weights:
            outMesh.add_weight(v4(values[index + 0], values[index + 1], values[index + 2], values[index + 3]));
            break;
        case cgltf_attribute_type_normal:
        {
            const auto normal = v3(values[index + 0], values[index + 1], values[index + 2]);
            outMesh.add_normal(normal.get_normalized().value_or(kk::up));
        }
        break;
        case cgltf_attribute_type_joints:
        {
            if (skin != nullptr)
            {
                const auto joint_at = [&](std::size_t ii) -> std::size_t
                {
                    const auto found = static_cast<int>(values[index + 0] + 0.5f);
                    const auto ret = static_cast<int>(std::max<std::size_t>(
                        0, find_node_index(skin->joints[found], nodes, nodeCount).value_or(0)
                    ));
                    return ret;
                };
                outMesh.add_influence({joint_at(0), joint_at(1), joint_at(2), joint_at(3)});
            }
            else
            {
                LOG_WARN("Mesh has joints but is missing skin");
            }
        }
        break;
        default:
            //ignore
            break;
        }
    }
}

core::geom::Vertex vert(std::size_t index)
{
    return core::geom::Vertex{index, index};
}

core::Geom create_geom_from_gltf_primitive(const cgltf_data& data, const cgltf_node& node, const cgltf_primitive& prim)
{
    core::geom::Builder mesh;

    cgltf_node* nodes = data.nodes;
    const auto nodeCount = data.nodes_count;
    const auto numAttributes = prim.attributes_count;
    for (unsigned int k = 0; k < numAttributes; ++k)
    {
        cgltf_attribute* attribute = &prim.attributes[k];
        extract_mesh_from_attribute(
            mesh, *attribute, node.skin, nodes, static_cast<unsigned int>(nodeCount)
        );
    }
    if (prim.indices != nullptr)
    {
        mesh.faces.reserve(prim.indices->count / 3);
        const auto indexCount = prim.indices->count;

        for (std::size_t k = 0; k < indexCount; k += 3)
        {
            const auto a1 = cgltf_accessor_read_index(prim.indices, k);
            const auto b2 = cgltf_accessor_read_index(prim.indices, k + 1);
            const auto c3 = cgltf_accessor_read_index(prim.indices, k + 2);
            mesh.add_face({ vert(a1), vert(b2), vert(c3) });
        }
    }

    return mesh.to_geom();
}

m4 transform_from_node_rec(const cgltf_node& node)
{
    const m4 parent = node.parent ? transform_from_node_rec(*node.parent) : m4_identity;
    const m4 translate = node.has_translation ? m4::from_translation(v3(node.translation)) : m4_identity;
    const m4 rotate = node.has_rotation ? m4::from(Q(node.rotation)).value_or(m4_identity) : m4_identity;
    const m4 scale = node.has_scale ? m4::from_scale(v3(node.scale)) : m4_identity;

    const auto transform = translate * rotate * scale;
    return parent * transform;
}

core::Mesh extract_meshes_from_gltf(const cgltf_data& data, const std::string& file)
{
    core::Mesh result;

    for (std::size_t i = 0; i < data.nodes_count; ++i)
    {
        const cgltf_node& node = data.nodes[i];
        if (node.mesh == nullptr)
        {
            continue;
        }

        core::TransformedMesh transform;
        transform.name = node.mesh->name != nullptr ? node.mesh->name : "unnamed_mesh";
        transform.transform = transform_from_node_rec(node);

        for (std::size_t primtive_index = 0; primtive_index < node.mesh->primitives_count; ++primtive_index)
        {
            const auto mesh = create_geom_from_gltf_primitive(data, node, node.mesh->primitives[primtive_index]);
            if (mesh.faces.empty() || mesh.vertices.empty())
            {
                LOG_WARN("Mesh has no mesh data in gltf file: {}", file);
            }
            else
            {
                transform.geoms.emplace_back(mesh);
            }
        }

        result.meshes.emplace_back(std::move(transform));
    }

    return result;
}

core::Mesh unable_to_load_geom()
{
    return
    {
        .meshes = {
            core::TransformedMesh
            {
                .name = "unable_to_load_geom",
                .transform = m4_identity,
                .geoms = {
                    core::MeshGeom
                    {
                        .geom = eu::core::geom::create_box(1.0f, 1.0f, 1.0f, core::geom::NormalsFacing::Out).to_geom()
                    }
                }
            }
        }
    };
}

core::Mesh mesh_from_file(const std::string& file)
{
    GltfFile gltf;
    if (gltf.load_gltf_file(file) == false)
    {
        LOG_ERR("Failed to load gltf file: {}", file);
        return unable_to_load_geom();
    }

    const auto ret = extract_meshes_from_gltf(*gltf.data, file);
    if (ret.meshes.empty())
    {
        LOG_ERR("No meshes found in gltf file: {}", file);
        return unable_to_load_geom();
    }
    
    return ret;
}

}

