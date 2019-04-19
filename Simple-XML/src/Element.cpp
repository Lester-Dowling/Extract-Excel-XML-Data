/**
 * @file   Simple-XML/Element.cpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element.hpp"
namespace simple_xml {

	void Element::set_name(vector<char> const& y)
	{
		using Iterator = vector<char>::const_iterator;
		Iterator ybeg = y.begin();
		Iterator yend = y.end();
		while (ybeg != yend && std::isspace(static_cast<unsigned char>(*ybeg)))
			++ybeg;
		if (ybeg != yend) {
			--yend;
			while (ybeg != yend && std::isspace(static_cast<unsigned char>(*yend)))
				--yend;
			++yend;
			m_name.assign(ybeg, yend);
		}
	}
	
	void Element::set_name(string const& y)
	{
		using Iterator = string::const_iterator;
		Iterator ybeg = y.begin();
		Iterator yend = y.end();
		while (ybeg != yend && std::isspace(static_cast<unsigned char>(*ybeg)))
			++ybeg;
		if (ybeg != yend) {
			--yend;
			while (ybeg != yend && std::isspace(static_cast<unsigned char>(*yend)))
				--yend;
			++yend;
			m_name.assign(ybeg, yend);
		}
	}


	void Element::set_text(vector<char> const& y)
	{
		using Iterator = vector<char>::const_iterator;
		Iterator ybeg = y.begin();
		Iterator yend = y.end();
		while (ybeg != yend && std::isspace(static_cast<unsigned char>(*ybeg)))
			++ybeg;
		if (ybeg != yend) {
			--yend;
			while (ybeg != yend && std::isspace(static_cast<unsigned char>(*yend)))
				--yend;
			++yend;
			m_text.assign(ybeg, yend);
		}
	}


	void Element::set_text(string const& y)
	{
		using Iterator = string::const_iterator;
		Iterator ybeg = y.begin();
		Iterator yend = y.end();
		while (ybeg != yend && std::isspace(static_cast<unsigned char>(*ybeg)))
			++ybeg;
		if (ybeg != yend) {
			--yend;
			while (ybeg != yend && std::isspace(static_cast<unsigned char>(*yend)))
				--yend;
			++yend;
			m_text.assign(ybeg, yend);
		}
	}


	optional<string> Element::attribute(string attribute_name) const
	{
		const auto fitr = attributes.find(attribute_name);
		if (fitr != attributes.end())
			return fitr->second;
		else
			return {};
	}

} // namespace simple_xml
