#include "Configuration.hpp"

#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

pg::Configuration::Configuration():
	uiBG(0xFFFFFFFF), uiTerminalBG(0xFFFFFFFF), uiTerminalShowSystemLevel(false),
	uiWaveformBG(0xFF000000), uiWaveformCore(0xFFFFFFFF), uiWaveformEdge(0xFFFFAA88)
{
}

#define CONFIG_READ(op, key, tree, name, Type) \
	if ((op = tree->get<Type>(name, key))) key = *op;


bool pg::Configuration::loadFile()
{
	std::cout << "Reading configuration from: " << fileName << std::endl;
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open()) return false;
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(file, tree,
			boost::property_tree::xml_parser::trim_whitespace);
	file.close();
	
	boost::optional<bool> keyBool;
	boost::optional<boost::property_tree::ptree&> treeUI =
		tree.get_child_optional("ui");
	if (treeUI)
	{
		boost::optional<Colour32> keyColour32;
		CONFIG_READ(keyColour32, uiBG, treeUI, "BG", Colour32);
		CONFIG_READ(keyColour32, uiTerminalBG, treeUI, "terminalBG", Colour32);
		CONFIG_READ(keyBool, uiTerminalShowSystemLevel, treeUI, "ShowSystemLevel", bool);
		CONFIG_READ(keyColour32, uiWaveformBG, treeUI, "waveformBG", Colour32);
		CONFIG_READ(keyColour32, uiWaveformCore, treeUI, "waveformCore", Colour32);
		CONFIG_READ(keyColour32, uiWaveformEdge, treeUI, "waveformEdge", Colour32);
	}



	return true;
}
void pg::Configuration::saveFile()
{
	std::cout << "Writing configuration to: " << fileName << std::endl;
	boost::property_tree::ptree tree;
	boost::property_tree::ptree treeUI;
	treeUI.put("BG", uiBG);
	treeUI.put("terminalBG", uiTerminalBG);
	treeUI.put("ShowSystemLevel", uiTerminalShowSystemLevel);
	treeUI.put("waveformBG", uiWaveformBG);
	treeUI.put("waveformCore", uiWaveformCore);
	treeUI.put("waveformEdge", uiWaveformEdge);
	tree.put_child("ui", treeUI);
	
	boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
	boost::property_tree::write_xml(fileName, tree, std::locale(), settings);
}
