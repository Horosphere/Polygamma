#include "Configuration.hpp"

#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pg
{

Configuration::Configuration():
	cacheDirPlayback(),
	uiBG(0xFFFFFFFF), uiTerminalBG(0xFFFFFFFF), uiScriptLevelMin(Script::UI),
	uiWaveformBG(0xFF000000), uiWaveformCore(0xFFFFFFFF), uiWaveformEdge(0xFFFFAA88)
{
}

// XML Convention: Trees start with lower case letter, entries start with upper
// case letter
bool Configuration::loadFile()
{
	std::cout << "Reading configuration from: " << fileName << std::endl;
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open()) return false;
	boost::property_tree::ptree tree;
	boost::property_tree::read_xml(file, tree,
	                               boost::property_tree::xml_parser::trim_whitespace);
	file.close();

	boost::optional<boost::property_tree::ptree&> treeCache =
	  tree.get_child_optional("cache");
	if (treeCache)
	{
		cacheDirPlayback = treeCache->get("cacheDirPlayback", cacheDirPlayback);
	}
	boost::optional<boost::property_tree::ptree&> treeUI =
	  tree.get_child_optional("ui");
	if (treeUI)
	{
		uiBG = treeUI->get("BG", uiBG);
		uiTerminalBG = treeUI->get("TerminalBG", uiTerminalBG);
		uiScriptLevelMin = (Script::Level) treeUI->get<int>("ScriptLevelMin", uiScriptLevelMin);
		uiWaveformBG = treeUI->get("WaveformBG", uiWaveformBG);
		uiWaveformCore = treeUI->get("WaveformCore", uiWaveformCore);
		uiWaveformEdge = treeUI->get("WaveformEdge", uiWaveformEdge);
	}

	return true;
}
void Configuration::saveFile()
{
	std::cout << "Writing configuration to: " << fileName << std::endl;
	boost::property_tree::ptree tree;

	boost::property_tree::ptree treeCache;
	treeCache.put("cacheDirPlayback", cacheDirPlayback);

	boost::property_tree::ptree treeUI;
	treeUI.put("BG", uiBG);
	treeUI.put("TerminalBG", uiTerminalBG);
	treeUI.put("ScriptLevelMin", uiScriptLevelMin);
	treeUI.put("WaveformBG", uiWaveformBG);
	treeUI.put("WaveformCore", uiWaveformCore);
	treeUI.put("WaveformEdge", uiWaveformEdge);
	tree.put_child("ui", treeUI);

	boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
	boost::property_tree::write_xml(fileName, tree, std::locale(), settings);
}

} // namespace pg
