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

        const auto bytes = bytes_from_file(path);

        // cgltf_result result = cgltf_parse_file(&options, path, &data);
        cgltf_result result = cgltf_parse(&options, bytes.data(), bytes.size(), &data);
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
            outMesh.add_text_coord(v2(values[index + 0], values[index + 1]));
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
            // These indices are skin relative. This function has no information about the
            // skin that is being parsed. Add +0.5f to round, since we can't read ints
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

std::vector<core::geom::Builder> extract_meshes_from_gltf(cgltf_data* data)
{
    std::vector<core::geom::Builder> result;
    cgltf_node* nodes = data->nodes;
    const auto nodeCount = data->nodes_count;

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
        cgltf_node* node = &nodes[i];
        if (node->mesh == nullptr || node->skin == nullptr)
        {
            continue;
        }
        const auto numPrims = node->mesh->primitives_count;
        for (std::size_t j = 0; j < numPrims; ++j)
        {
            result.emplace_back();
            auto& mesh = result[result.size() - 1];

            cgltf_primitive* primitive = &node->mesh->primitives[j];

            const auto numAttributes = primitive->attributes_count;
            for (unsigned int k = 0; k < numAttributes; ++k)
            {
                cgltf_attribute* attribute = &primitive->attributes[k];
                extract_mesh_from_attribute(
                    mesh, *attribute, node->skin, nodes, static_cast<unsigned int>(nodeCount)
                );
            }
            if (primitive->indices != nullptr)
            {
                mesh.faces.reserve(primitive->indices->count / 3);
                const auto indexCount = primitive->indices->count;

                for (std::size_t k = 0; k < indexCount; k+=3)
                {
                    const auto a1 = cgltf_accessor_read_index(primitive->indices, k);
                    const auto b2 = cgltf_accessor_read_index(primitive->indices, k + 1);
                    const auto c3 = cgltf_accessor_read_index(primitive->indices, k + 2);
                    mesh.add_face({vert(a1), vert(b2), vert(c3)});
                }
            }
        }
    }

    return result;
}

core::Geom unable_to_load_geom()
{
    return eu::core::geom::create_box(1.0f, 1.0f, 1.0f, core::geom::NormalsFacing::Out).to_geom();
}

core::Geom gom_from_file(const std::string& file)
{
    GltfFile gltf;
    if (gltf.load_gltf_file(file) == false)
    {
        LOG_ERR("Failed to load gltf file: {}", file);
        return unable_to_load_geom();
    }

    const auto meshes = extract_meshes_from_gltf(gltf.data);
    if (meshes.empty())
    {
        LOG_ERR("No meshes found in gltf file: {}", file);
        return unable_to_load_geom();
    }
    if (meshes.size() != 1)
    {
        LOG_WARN("Expected exactly one mesh in gltf file: {} but found {}", file, meshes.size());
    }
    const auto ret = meshes[0].to_geom();
    if (ret.faces.empty() || ret.vertices.empty())
    {
        LOG_ERR("Mesh has mesh data in gltf file: {}", file);
        return unable_to_load_geom();
    }
    return ret;
}

}

