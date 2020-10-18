// SPDX-License-Identifier: GPL-2.0-or-later

#include "xml_support.h"

#include <gtest/gtest.h>

#include <libxml/parser.h>

char const*const xmlFile = R"(<?xml version="1.0"?>
<foo>
  Lorem Ipsum!
  <bar>
    <baz1/>
    <baz2/>
    <baz3/>
  </bar>
</foo>)";

struct TegXmlTest: public ::testing::Test
{
	xmlDocPtr doc = nullptr;
	xmlNodePtr root = nullptr;

	void SetUp() override {
		doc = xmlParseDoc(reinterpret_cast<xmlChar const*>(xmlFile));
		ASSERT_NE(nullptr, doc) << "Sample text parsing failed";
		root = xmlDocGetRootElement(doc);
		ASSERT_NE(nullptr, root) << "The root element could not be found";
	}

	void TearDown() override {
		if(doc != nullptr) {
			xmlFreeDoc(doc);
		}
		root = nullptr;
		doc = nullptr;
	}
};

TEST_F(TegXmlTest, get_child)
{
	xmlNodePtr const bar = xml_get_element_children(root);
	ASSERT_NE(nullptr, bar);
	EXPECT_STREQ("bar", reinterpret_cast<char const*>(bar->name));
}

TEST_F(TegXmlTest, get_sibling)
{
	xmlNodePtr const bar = xml_get_element_children(root);
	ASSERT_NE(nullptr, bar);

	xmlNodePtr const baz1 = xml_get_element_children(bar);
	ASSERT_NE(nullptr, baz1);
	EXPECT_STREQ("baz1", reinterpret_cast<char const*>(baz1->name));

	xmlNodePtr const baz2 = xml_get_element_next(baz1);
	ASSERT_NE(nullptr, baz2);
	EXPECT_STREQ("baz2", reinterpret_cast<char const*>(baz2->name));

	xmlNodePtr const baz3 = xml_get_element_next(baz2);
	ASSERT_NE(nullptr, baz3);
	EXPECT_STREQ("baz3", reinterpret_cast<char const*>(baz3->name));

	xmlNodePtr const nothing = xml_get_element_next(baz3);
	EXPECT_EQ(nullptr, nothing);
}

TEST_F(TegXmlTest, add_numeric_attribute)
{
	add_numeric_attribute(root, "zero", 0);
	add_numeric_attribute(root, "min", std::numeric_limits<int>::min());
	add_numeric_attribute(root, "max", std::numeric_limits<int>::max());
	add_numeric_attribute(root, "twentythree", 23);

	auto gp = [this](const char* name) -> std::string {
		auto value = xmlGetProp(root, reinterpret_cast<xmlChar const*>(name));
		if (value != nullptr)
			return std::string(reinterpret_cast<char *>(value));
		else {
			return "<NULL>";
		}
	};

	EXPECT_EQ("0", gp("zero"));
	EXPECT_EQ(std::to_string(std::numeric_limits<int>::min()), gp("min"));
	EXPECT_EQ(std::to_string(std::numeric_limits<int>::max()), gp("max"));
	EXPECT_EQ("23", gp("twentythree"));
}
