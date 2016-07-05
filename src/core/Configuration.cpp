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
		uiBG = treeUI->get("BG", uiBG);
		uiTerminalBG = treeUI->get("terminalBG", uiTerminalBG);
		uiTerminalShowSystemLevel = treeUI->get("ShowSystemLevel", uiTerminalShowSystemLevel);
		uiWaveformBG = treeUI->get("waveformBG", uiWaveformBG);
		uiWaveformCore = treeUI->get("waveformCore", uiWaveformCore);
		uiWaveformEdge = treeUI->get("waveformEdge", uiWaveformEdge);
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
