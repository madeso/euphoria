#include "eu/io/mesh.h"

#include "eu/core/geom.builder.h"

#include "eu/io/file.h"

#include "cgltf.h"

namespace eu::io
{

namespace
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
        constexpr cgltf_options options = {};
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
    cgltf_node* target, cgltf_node* all_nodes, std::size_t node_count
)
{
    if (target == nullptr)
    {
        return std::nullopt;
    }
    for (std::size_t node_index = 0; node_index < node_count; ++node_index)
    {
        if (target == &all_nodes[node_index])
        {
            return node_index;
        }
    }
    return std::nullopt;
}

std::vector<float> scalar_values_from_accessor(
    std::size_t component_count, const cgltf_accessor& accessor
)
{
    std::vector<float> ret;
    ret.resize(accessor.count * component_count);

    for (cgltf_size accessor_index = 0; accessor_index < accessor.count; ++accessor_index)
    {
        cgltf_accessor_read_float(&accessor, accessor_index, &ret[accessor_index * component_count], component_count);
    }
    return ret;
}

void extract_mesh_from_attribute(
    core::geom::Builder& out_mesh,
    cgltf_attribute& attribute,
    cgltf_skin* skin,
    cgltf_node* nodes,
    unsigned int node_count
)
{
    const cgltf_attribute_type attrib_type = attribute.type;
    const cgltf_accessor& accessor = *attribute.data;

    unsigned int component_count = 0;
    if (accessor.type == cgltf_type_vec2)
    {
        component_count = 2;
    }
    else if (accessor.type == cgltf_type_vec3)
    {
        component_count = 3;
    }
    else if (accessor.type == cgltf_type_vec4)
    {
        component_count = 4;
    }
    const auto values = scalar_values_from_accessor(component_count, accessor);
    const auto accessor_count = accessor.count;

    for (std::size_t accessor_index = 0; accessor_index < accessor_count; ++accessor_index)
    {
        const auto index = accessor_index * component_count;
        switch (attrib_type)
        {
        case cgltf_attribute_type_position:
            out_mesh.add_position(v3(values[index + 0], values[index + 1], values[index + 2])
            );
            break;
        case cgltf_attribute_type_texcoord:
            // note: eu 2d coordinate system is bottom left going up, gltf is top left going down, `1-y` fixes this
            out_mesh.add_text_coord(v2(values[index + 0], 1-values[index + 1]));
            break;
        case cgltf_attribute_type_weights:
            out_mesh.add_weight(v4(values[index + 0], values[index + 1], values[index + 2], values[index + 3]));
            break;
        case cgltf_attribute_type_normal:
        {
            const auto normal = v3(values[index + 0], values[index + 1], values[index + 2]);
            out_mesh.add_normal(normal.get_normalized().value_or(kk::up));
        }
        break;
        case cgltf_attribute_type_joints:
        {
            if (skin != nullptr)
            {
                const auto joint_at = [&](std::size_t joint_index) -> std::size_t
                {
                    const auto found = std::lround(values[index + joint_index]);
                    const auto ret = static_cast<int>(std::max<std::size_t>(
                        0, find_node_index(skin->joints[found], nodes, node_count).value_or(0)
                    ));
                    return ret;
                };
                out_mesh.add_influence({joint_at(0), joint_at(1), joint_at(2), joint_at(3)});
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
    const auto node_count = data.nodes_count;

    const auto attribute_count = prim.attributes_count;
    for (unsigned int attribute_index = 0; attribute_index < attribute_count; ++attribute_index)
    {
        cgltf_attribute* attribute = &prim.attributes[attribute_index];
        extract_mesh_from_attribute(
            mesh, *attribute, node.skin, nodes, static_cast<unsigned int>(node_count)
        );
    }
    if (prim.indices != nullptr)
    {
        mesh.faces.reserve(prim.indices->count / 3);
        const auto index_count = prim.indices->count;

        for (std::size_t index_index = 0; index_index < index_count; index_index += 3)
        {
            const auto a1 = cgltf_accessor_read_index(prim.indices, index_index);
            const auto b2 = cgltf_accessor_read_index(prim.indices, index_index + 1);
            const auto c3 = cgltf_accessor_read_index(prim.indices, index_index + 2);
            mesh.add_face({ vert(a1), vert(b2), vert(c3) });
        }
    }

    return mesh.to_geom();
}

m4 transform_from_node_rec(const cgltf_node& node)
{
    const m4 parent = node.parent != nullptr ? transform_from_node_rec(*node.parent) : m4_identity;
    const m4 translate = node.has_translation != 0 ? m4::from_translation(v3(node.translation)) : m4_identity;
    const m4 rotate = node.has_rotation != 0 ? m4::from(Q(node.rotation)).value_or(m4_identity) : m4_identity;
    const m4 scale = node.has_scale != 0 ? m4::from_scale(v3(node.scale)) : m4_identity;

    const auto transform = translate * rotate * scale;
    return parent * transform;
}

core::Mesh extract_meshes_from_gltf(const cgltf_data& data, const std::string& file)
{
    core::Mesh result;

    for (std::size_t node_index = 0; node_index < data.nodes_count; ++node_index)
    {
        const cgltf_node& node = data.nodes[node_index];
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

