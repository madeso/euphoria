#include "eu/core/vertex_layout.h"

#include <algorithm>

#include "eu/assert/assert.h"

namespace internal
{
    namespace
    {
        using namespace eu;
        using namespace eu::core;


        /** A list of things we need to extract from the Geom when compiling */
        struct VertexTypeList
        {
            explicit VertexTypeList(const std::vector<VertexType>& a_base_layout)
                : base_layout(a_base_layout)
            {}

            void add(const ShaderVertexAttributes& elements)
            {
                for (const auto& e : elements)
                {
                    indices.insert(e.type);
                }
            }

            std::vector<VertexType> base_layout;
            std::set<VertexType> indices;
        };


        int calculate_shader_attribute_size(const VertexType&)
        {
            return 1;
        }

        CompiledVertexTypeList compile_vertex_type_list(const VertexTypeList& list)
        {
            std::map<VertexType, int> indices;

            int next_index = 0;

            for (const auto type : list.base_layout)
            {
                indices.insert({ type, next_index });
                next_index += calculate_shader_attribute_size(type);
            }

            for (const auto type : list.indices)
            {
                // already in base layout, don't add again
                if (indices.contains(type))
                {
                    continue;
                }

                indices.insert({ type, next_index });
                next_index += calculate_shader_attribute_size(type);
            }

            return {
                .indices = indices,
                .debug_types = {list.indices.begin(), list.indices.end()},
                .next_index = next_index
            };
        }
    }
}

namespace eu::core
{


std::vector<VertexType> find_duplicates(const ShaderVertexAttributes& attributes)
{
	std::vector<VertexType> ret;
	
	std::set<VertexType> seen;
	for (const auto& att : attributes)
	{
		if (seen.insert(att.type).second == false)
		{
			ret.emplace_back(att.type);
		}
	}

	return ret;
}


CompiledShaderVertexAttributes compile_shader_layout(
	const CompiledVertexTypeList& l,
	const ShaderVertexAttributes& elements,
	std::optional<InstanceProp> instance_prop,
	std::optional<int> start_index
)
{
	std::vector<CompiledVertexElement> list;

	for (const auto& e: elements)
	{
		const auto found = l.indices.find(e.type);
		ASSERT(found != l.indices.end() && "layout wasn't added to the compilation list");
		if (found != l.indices.end())
		{
			list.push_back({
                .type = e.type,
                .name = e.name,
                .index = found->second
			});
		}
	}

	if (instance_prop.has_value())
	{
		// if the value was provided, it can't be less than what we have already used
		ASSERT(start_index.has_value() == false || start_index.value() >= l.next_index);
		list.push_back({
            .type = instance_prop->type,
            .name = instance_prop->name,
            .index = start_index.value_or(l.next_index)
		});
	}

	return {
	    .elements = list,
        .debug_types = l.debug_types
	};
}

[[nodiscard]]
CompiledGeomVertexAttributes get_geom_layout(const CompiledVertexTypeList& l)
{
	std::vector<CompiledVertexElementNoName> list;
	list.reserve(l.indices.size());

	for (const auto& [first, second]: l.indices)
	{
		list.push_back({
            .type = first,
            .index = second
		});
	}

	std::sort(
		list.begin(),
		list.end(),
		[](const CompiledVertexElementNoName& lhs, const CompiledVertexElementNoName& rhs)
		{ return lhs.index < rhs.index; }
	);

	// todo(Gustav): the index property is confusing and error-prone, just remove it

	return {
	    .elements = list,
        .debug_types = l.debug_types
	};
}


CompiledVertexTypeList compile_attribute_layouts(
	const std::vector<VertexType>& base_layout, const std::vector<ShaderVertexAttributes>& descriptions
)
{
	auto list = internal::VertexTypeList{base_layout};

	for (const auto& d: descriptions)
	{
		const auto duplicates = find_duplicates(d);
		ASSERT(duplicates.empty() && "ShaderVertexAttributes has duplicate types");
		list.add(d);
	}

	return compile_vertex_type_list(list);
}

CompiledVertexTypeList compile_attribute_layouts(const std::vector<ShaderVertexAttributes>& descriptions)
{
	return compile_attribute_layouts({}, descriptions);
}

}  //  namespace eu::core
