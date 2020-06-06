#pragma comment(lib, "ws2_32.lib")
#include "SServer.h"
using namespace std;


SServer::SServer()
{
	client_count = 0;
}

void SServer::autoSave()
{
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(60));
		opDataBase::Hinstance().saveAll();
		cout << "�������������� �������..." << endl;
	}
}

SServer::~SServer()
{
}

void SServer::startServer() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) /*return 0*/; // ��� ����������� ��
	s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);
	local_addr.sin_addr.s_addr = 0;
	bind(s, (sockaddr*)&local_addr, sizeof(local_addr));
	if (s == SOCKET_ERROR) {
		cout << "����� �� ��� ������\n";
	}

	if (::bind(s, (sockaddr*)&local_addr, sizeof(local_addr)) != SOCKET_ERROR) {
		printf("����� ������� ��������\n");
	}

	if (listen(s, 5) != SOCKET_ERROR) {
		printf("����� ������������� �����: %u\n", ntohs(local_addr.sin_port));
	}
	SOCKET client_socket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	opDataBase::Hinstance().uploadAll();
	thread save(&SServer::autoSave, this);
	save.detach();
	while (client_socket = accept(s, (sockaddr*)&client_addr, &client_addr_size))
	{
		client_count++;
		cout << "����������� ����� ������!" << endl;
		cout << "����� ����� ��������: " << client_count << endl;
		thread th(&SServer::handle, this , client_socket);
		th.detach();
	}
}

void SServer::closeServer() {
	closesocket(s);
	WSACleanup();
	cout << "������ ��� ����������." << endl;
}


void SServer::handle(SOCKET client_socket) {
	ClientInterface clInterf(client_socket);
	cout << "����� " << this_thread::get_id() << " ����� ������ � ��������." << endl;
	try {
		clInterf.startMenu();
	}
	catch (BREAK_THREAD_EX ex)
	{
		cout << "����� " << setw(5) << this_thread::get_id() << " : " << ex.what_is() << endl;
	}
	closesocket(client_socket);
	client_count--;
	cout << "���������� ��������� ������������ �� ������ " << this_thread::get_id() << "..." << endl;
	opDataBase::Hinstance().saveAll();
	cout << "����� " << this_thread::get_id() << " �������� ������." << endl;
	cout << "����� ����� ��������: " << client_count << endl;
}


void SServer::setPort(unsigned short port)
{
	this->port = port;
}