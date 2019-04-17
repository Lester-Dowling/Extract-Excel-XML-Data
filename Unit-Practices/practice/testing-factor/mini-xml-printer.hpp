#pragma once
#include <iostream>

namespace learning {
	///////////////////////////////////////////////////////////////////////////
	//  Print out the mini xml tree
	///////////////////////////////////////////////////////////////////////////
	int const tabsize = 4;

	void tab(int indent)
	{
		for (int i = 0; i < indent; ++i)
			std::cout << ' ';
	}

	struct mini_xml_printer
	{
		mini_xml_printer(int indent = 0)
		  : indent(indent)
		{}

		void operator()(mini_xml const& xml) const;

		int indent;
	};

	struct mini_xml_node_printer : boost::static_visitor<>
	{
		mini_xml_node_printer(int indent = 0)
		  : indent(indent)
		{}

		void operator()(mini_xml const& xml) const
		{
			mini_xml_printer(indent + tabsize)(xml);
		}

		void operator()(std::string const& text) const
		{
			tab(indent + tabsize);
			std::cout << "text: \"" << text << '"' << std::endl;
		}

		int indent;
	};

	void mini_xml_printer::operator()(mini_xml const& xml) const
	{
		tab(indent);
		std::cout << "tag: " << xml.tag.name << ", attributes: " << xml.tag.attributes
				  << std::endl;
		tab(indent);
		std::cout << '{' << std::endl;

		for (mini_xml_node const& node : xml.children) {
			boost::apply_visitor(mini_xml_node_printer(indent), node);
		}

		tab(indent);
		std::cout << '}' << std::endl;
	}
} // namespace learning
