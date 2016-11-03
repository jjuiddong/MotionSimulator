
#include "stdafx.h"
#include "udpanalyzeroption.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


// 전역 인스턴스
cUDPAnalyzerOption g_option;


cUDPAnalyzerOption::cUDPAnalyzerOption()
	: m_initWindows(false)
{
	m_plotViewCmd.resize(10);
	m_plotInputCmd.resize(10);

}

cUDPAnalyzerOption::~cUDPAnalyzerOption()
{

}


bool cUDPAnalyzerOption::Read(const string &fileName, const bool showMsgBox ) //showMsgBox=true
{

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName, props);
		m_fileName = fileName;
		m_com = props.get<int>("COM", -1);
		m_baudRate = props.get<int>("BAUDRATE", 9600);
		m_ip = props.get<string>("IP", "127.0.0.1");
		m_port = props.get<int>("PORT", 8888);
		m_sendType = props.get<int>("SENDTYPE", 1);
		m_sendFormat = props.get<string>("SENDFORMAT", "$1;$2;$3");
		m_rollCmd = props.get<string>("ROLLCMD", "$1");
		m_pitchCmd = props.get<string>("PITCHCMD", "$2");
		m_yawCmd = props.get<string>("YAWCMD", "$3");
		m_heaveCmd = props.get<string>("HEAVECMD", "$4");
		m_roll2Cmd = props.get<string>("ROLL2CMD", "$1");
		m_pitch2Cmd = props.get<string>("PITCH2CMD", "$2");
		m_yaw2Cmd = props.get<string>("YAW2CMD", "$3");
		m_heave2Cmd = props.get<string>("HEAVE2CMD", "$4");

		m_plotCmd = props.get<string>("PLOTCMD", "");

		m_mixingCmd = props.get<string>("MIXINGCMD", "");

		m_plotViewCmd.resize(10);
		m_plotInputCmd.resize(10);
		for (int i=0; i < 10; ++i)
		{
			stringstream ss1;
			ss1 << "PLOTVIEWCMD" << i + 1;
			m_plotViewCmd[i] = props.get<string>(ss1.str(), "");

			stringstream ss2;
			ss2 << "PLOTINPUTCMD" << i + 1;
			m_plotInputCmd[i] = props.get<string>(ss2.str(), "");
		}

		//m_plotViewCmd = props.get<string>("PLOTVIEWCMD", "");
		//m_plotInputCmd = props.get<string>("PLOTINPUTCMD", "");
		m_udpProtocolCmd = props.get<string>("UDPPROTOCOLCMD", "");
		m_udpPort = props.get<int>("UDPPORT", 8888);

		m_udpPlayerIP = props.get<string>("UDPPLAYERIP", "127.0.0.1");
		m_udpPlayerPort = props.get<int>("UDPPLAYERPORT", 8888);

		m_udpSendIp = props.get<string>("UDPSEND_IP", "127.0.0.1");
		m_udpSendPort = props.get<int>("UDPSEND_PORT", 8888);
		m_udpSendEdit[0] = props.get<string>("UDPSEND_EDIT1", "");
		m_udpSendEdit[1] = props.get<string>("UDPSEND_EDIT2", "");
		m_udpSendEdit[2] = props.get<string>("UDPSEND_EDIT3", "");
		m_udpSendEdit[3] = props.get<string>("UDPSEND_EDIT4", "");
		m_udpSendEdit[4] = props.get<string>("UDPSEND_EDIT5", "");
		m_udpSendEdit[5] = props.get<string>("UDPSEND_EDIT6", "");
		m_udpSendEdit[6] = props.get<string>("UDPSEND_EDIT7", "");
		m_udpSendEdit[7] = props.get<string>("UDPSEND_EDIT8", "");
		m_udpSendEdit[8] = props.get<string>("UDPSEND_EDIT9", "");
		m_udpSendEdit[9] = props.get<string>("UDPSEND_EDIT10", "");

		m_initWindows = props.get<bool>("INITWINDOWS", false);
	}
	catch (std::exception&e)
	{
		if (showMsgBox)
			::AfxMessageBox(CString(L"Error!!\n") + str2wstr(e.what()).c_str());
	}

	return true;
}


bool cUDPAnalyzerOption::Write(const string &fileName)
{
	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		props.add<int>("COM", m_com);
		props.add<int>("BAUDRATE", m_baudRate);
		props.add<string>("IP", m_ip);
		props.add<int>("PORT", m_port);
		props.add<int>("SENDTYPE", m_sendType);
		props.add<string>("SENDFORMAT", m_sendFormat);
		props.add<string>("ROLLCMD", m_rollCmd);
		props.add<string>("PITCHCMD", m_pitchCmd);
		props.add<string>("YAWCMD", m_yawCmd);
		props.add<string>("HEAVECMD", m_heaveCmd);

		props.add<string>("ROLL2CMD", m_roll2Cmd);
		props.add<string>("PITCH2CMD", m_pitch2Cmd);
		props.add<string>("YAW2CMD", m_yaw2Cmd);
		props.add<string>("HEAVE2CMD", m_heave2Cmd);
		
		props.add<string>("PLOTCMD", m_plotCmd);

		props.add<string>("MIXINGCMD", m_mixingCmd);

		for (u_int i = 0; i < m_plotViewCmd.size(); ++i)
		{
			stringstream ss1;
			ss1 << "PLOTVIEWCMD" << i + 1;
			props.add<string>(ss1.str(), m_plotViewCmd[i]);

			stringstream ss2;
			ss2 << "PLOTINPUTCMD" << i + 1;
			props.add<string>(ss2.str(), m_plotInputCmd[i]);
		}
// 		props.add<string>("PLOTVIEWCMD", m_plotViewCmd);
// 		props.add<string>("PLOTINPUTCMD", m_plotInputCmd);
		
		props.add<string>("UDPPROTOCOLCMD", m_udpProtocolCmd);
		props.add<int>("UDPPORT", m_udpPort);
		props.add<string>("UDPPLAYERIP", m_udpPlayerIP);
		props.add<int>("UDPPLAYERPORT", m_udpPlayerPort);

		props.add<string>("UDPSEND_IP", m_udpSendIp);
		props.add<int>("UDPSEND_PORT", m_udpSendPort);
		props.add<string>("UDPSEND_EDIT1", m_udpSendEdit[0]);
		props.add<string>("UDPSEND_EDIT2", m_udpSendEdit[1]);
		props.add<string>("UDPSEND_EDIT3", m_udpSendEdit[2]);
		props.add<string>("UDPSEND_EDIT4", m_udpSendEdit[3]);
		props.add<string>("UDPSEND_EDIT5", m_udpSendEdit[4]);
		props.add<string>("UDPSEND_EDIT6", m_udpSendEdit[5]);
		props.add<string>("UDPSEND_EDIT7", m_udpSendEdit[6]);
		props.add<string>("UDPSEND_EDIT8", m_udpSendEdit[7]);
		props.add<string>("UDPSEND_EDIT9", m_udpSendEdit[8]);
		props.add<string>("UDPSEND_EDIT10", m_udpSendEdit[9]);

		props.add<bool>("INITWINDOWS", m_initWindows);

		boost::property_tree::write_json(fileName, props);
	}
	catch (std::exception&e)
	{
		::AfxMessageBox(CString(L"Error!!\n") + str2wstr(e.what()).c_str());
	}

	return true;
}
