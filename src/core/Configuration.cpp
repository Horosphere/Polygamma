#include "Configuration.hpp"

#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

pg::Configuration::Configuration():
	ioAudioDeviceInput(), ioAudioDeviceOutput(),
	uiBG(0xFFFFFFFF), uiTerminalBG(0xFFFFFFFF), uiTerminalShowSystemLevel(true),
	uiWaveformBG(0xFF000000), uiWaveformCore(0xFFFFFFFF), uiWaveformEdge(0xFFFFAA88)
{
}

// XML Convention: Trees start with lower case letter, entries start with upper
// case letter
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
	
	boost::optional<boost::property_tree::ptree&> treeIO =
		tree.get_child_optional("io");
	if (treeIO)
	{
		ioAudioDeviceInput = treeIO->get("AudioDeviceInput", ioAudioDeviceInput);
		ioAudioDeviceOutput = treeIO->get("AudioDeviceOutput", ioAudioDeviceOutput);
	}
	boost::optional<boost::property_tree::ptree&> treeUI =
		tree.get_child_optional("ui");
	if (treeUI)
	{
		uiBG = treeUI->get("BG", uiBG);
		uiTerminalBG = treeUI->get("TerminalBG", uiTerminalBG);
		uiTerminalShowSystemLevel = treeUI->get("ShowSystemLevel", uiTerminalShowSystemLevel);
		uiWaveformBG = treeUI->get("WaveformBG", uiWaveformBG);
		uiWaveformCore = treeUI->get("WaveformCore", uiWaveformCore);
		uiWaveformEdge = treeUI->get("WaveformEdge", uiWaveformEdge);
	}

	return true;
}
void pg::Configuration::saveFile()
{
	std::cout << "Writing configuration to: " << fileName << std::endl;
	boost::property_tree::ptree tree;

	boost::property_tree::ptree treeIO;
	treeIO.put("AudioDeviceInput", ioAudioDeviceInput);
	treeIO.put("AudioDeviceInput", ioAudioDeviceOutput);

	boost::property_tree::ptree treeUI;
	treeUI.put("BG", uiBG);
	treeUI.put("TerminalBG", uiTerminalBG);
	treeUI.put("ShowSystemLevel", uiTerminalShowSystemLevel);
	treeUI.put("WaveformBG", uiWaveformBG);
	treeUI.put("WaveformCore", uiWaveformCore);
	treeUI.put("WaveformEdge", uiWaveformEdge);
	tree.put_child("ui", treeUI);
	
	boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
	boost::property_tree::write_xml(fileName, tree, std::locale(), settings);
}
