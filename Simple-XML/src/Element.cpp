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


	std::string Element::all_fields_to_string() const
	{
		using std::endl;
		using std::setw;
		std::ostringstream oss;
		if (!name().empty()) {
			oss << "Element name:" << ' ' << ' ' << ' ' << ' ' << name() << endl;
		}
		if (!attributes.empty()) {
			oss << setw(16) << "Attr:" << ' ';
			for (auto attr : attributes) {
				oss << attr.first << '=' << attr.second << ' ' << ' ' << ' ';
			}
			oss << endl;
		}
		if (!text().empty()) {
			oss << setw(16) << "Text:" << ' ' << text() << endl;
		}
		oss << setw(16) << "Row:" << ' ' << row_idx << endl;
		oss << setw(16) << "Col:" << ' ' << col_idx << endl;
		return oss.str();
	}

} // namespace simple_xml
