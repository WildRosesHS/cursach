#include "SClient.h"
#pragma warning(disable:4996)
#define BREAK_THREAD 1


SClient::SClient()
{
	serverMsg = "";
}


SClient::~SClient()
{
}

void SClient::programInfo()
{
	system("cls");
	system("title � ���������");
	cout << "�������� ������ �� ���������� ������ ���������� ����� �����������\n����: \"����������� �������� ��� �������� ������� ������� ������� ��������� ��������� (�� ������� ��������� �����������)\"\n�����, 2020" << endl;
	system("pause");
}

int SClient::getMenuChoice(int countOfMenuBoxes, int numberOfFirst)
{
	int choice;
	cout << "������� ��� ����� - ";
	while (!(cin >> choice) || choice < numberOfFirst || choice > (numberOfFirst + countOfMenuBoxes - 1))
	{
		cout << "������������ �����! ��������� ������� - ";
		cin.clear();
		rewind(stdin);
	}
	return choice;
}
template <typename T>
T SClient::getNum()
{
	T choice;
	while (!(cin >> choice))
	{
		cout << "������������ �����! ��������� ������� - ";
		cin.clear();
		rewind(stdin);
	}
	return choice;
}

void SClient::connectToS()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0);
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(1024);
	dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while (connect(server_socket, (sockaddr*)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR)
	{
		cout << "�� ������� ����������� � ��������, ��������� ����������������� ������� � ��������� �������" << endl;
		system("pause");
	}
	try {
		startMenu();
	}
	catch (int)
	{
		cout << "������ ������� ����������.\n";
		system("pause");
	}
	closesocket(server_socket);
}
void SClient::sendToS(string str)
{
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	for (unsigned int i = 0; i < (str.length() / (BUFFER_SIZE-1)); i++)
	{
		buf[0] = '1';
		str.copy(buf + 1, BUFFER_SIZE-1, i * (BUFFER_SIZE-1));
		if (send(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD;
	}
	buf[0] = '0';
	str.copy(buf + 1, BUFFER_SIZE-1, str.length() / (BUFFER_SIZE - 1));
	buf[str.length() % (BUFFER_SIZE - 1)+1] = '\0';
	if (send(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD;

}
string SClient::recvFromS()
{
	string result = "";
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	if (recv(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD;
	result += buf + 1;
	while (buf[0] == 1)
	{
		if (recv(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD;
		result += buf + 1;
	}
	return result;

}

//��������� ����
void SClient::startMenu()
{
	bool back = 0;
	while (!back)
	{
		system("cls");
		system("title ��������� ����");
		cout << "��� ������������� ��������� ����� \"�����\"" << endl;
		cout << "1 - ����������� �������\n2 - ����������� �������\n0 - ����� �� ���������\n";
		switch (getMenuChoice(3))
		{
		case 1:
			auth();
			break;
		case 2:
			registration();
			break;
		case 0:
			sendToS("Exit");
			back = 1;
			break;
		}
	}
}



//�����������
void SClient::auth()
{	
	bool back = 0;
	sendToS("Start authorization");
	while (!back) {
		system("cls");
		system("title �����������");
		string temp;
		cout << "�����������:\n" << endl;
		cout << "�����: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		currentUser.setLogin(temp);
		sendToS(temp);
		cout << "������: ";
		getline(cin, temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Error: user not founded")
		{
			cout << "����������� ������ ����� ��� ������. ������� ����������?\n1 - ��\n0 - ���\n";
			if (!getMenuChoice(2))
				back = true;
		}
		else
		{
			currentUser.setAdminMode(stoi(recvFromS()));
			currentUser.setFName(recvFromS());
			currentUser.setLName(recvFromS());
			cout << "�� ���� ��������������." << endl;
			if (currentUser.getFName() != "")
				cout << "����� ���������� � ������ ��������� �����, " << currentUser.getFName() << "!\n";
			system("pause");
			if (currentUser.getAdminMode())
				back = adminMenu();
			else
				back = userMenu();
		}
	}
	sendToS("Back");
}



//����������� ������������
void SClient::registration()
{
	sendToS("Start registration");
	while (true)
	{
		system("title �����������");
		system("cls");
		string temp;
		cout << "������� �������: ";
		cin >> temp;
		currentUser.setLName(temp);
		sendToS(temp);
		cout << "������� ���: ";
		cin >> temp;
		sendToS(temp);
		currentUser.setFName(temp);
		cout << "������� �����: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setLogin(temp);
		cout << "������� ������: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		serverMsg = recvFromS();
		if (serverMsg == "Already exist")
		{
			cout << "������������ � ����� ������� � ������� ��� ����������!\n��������� �������?\n1 - ��\n0 - ���" << endl;
			if (!getMenuChoice(2))
				break;
		}
		if (serverMsg == "Successful registration")
		{
			cout << "�� ���� ����������������." << endl;
			system("pause");
			break;
		}
	}
	sendToS("Back");
}



//���� ��������������
bool SClient::adminMenu()
{
	bool back = false;
	sendToS("Start admin menu");
	while (!back)
	{
		system("title ���� ��������������");
		system("cls");
		cout << "�������������: " << currentUser.getFName() << "\n1 - ���������� ������� ��������\n2 - �������������� ������ � ��������\n3 - ����� � ���������� �������\n4 - ��������\n5 - ���������� ������\n0 - �����\n";
		switch (getMenuChoice(6))
		{
		case 1:
			userControlMenu();
			break;
		case 2:
			productsControlMenu();
			break;
		case 3:
			printFullReport();
			break;
		case 4:
			programInfo();
			break;
		case 5:
			saveData();
			break;
		case 0:
			sendToS("Back");
			back = true;

			break;
		}
	}
	return back;
}

//���� ������������
bool SClient::userMenu()
{
	bool back = false;
	sendToS("Start user menu");
	while (!back)
	{
		system("cls");
		system("title ���� ������������");
		cout << "������������ ���, " << currentUser.getFName() << "\n1 - ������� ��� ������\n2 - ������ ���� ��������\n3 - ������ ���� ����� ��������\n4 - ������� �����\n5 - ��������\n0 - �����" << endl;
		switch (getMenuChoice(6))
		{
		case 1:
			expertRateMenu();
			break;
		case 2:
			printProducts();
			break;
		case 3:
			infoGroups();
			break;
		case 4:
			printFullReport();
			break;
		case 5:
			programInfo();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
	return true;
}

//���� ���������� ������
void SClient::expertRateMenu()
{
	sendToS("Start expert rate menu");
	while (true) {
		system("cls");
		system("title ������ ����� �������");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "�� ������ �� ���� ������!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "�������� ����� ���� �������, ����� ����������� ���������� � ���" << endl;
			cout << setw(3) << "#" << setw(20) << "�������� ���� �������" << setw(15) << "������ ������" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - �����" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			cout << "������ ������ ���� �������?\n1 - ��\n0 - ���" << endl;
			choice = getMenuChoice(2);
			if (choice)
			{
				sendToS("Start rate");
				int maxScale = stoi(recvFromS());
				int count = stoi(recvFromS());
				for (int i = 0; i < count; i++)
				{
					system("cls");
					for (int j = i + 1; j < count; j++)
					{
						cout << recvFromS();
						cout << recvFromS();
						cout << recvFromS() << endl;
						sendToS(to_string(getMenuChoice(maxScale)));
						system("cls");
					}
				}
				cout << recvFromS() << endl;
				system("pause");
			}
			else
			{
				break;
			}
		}
	}
	sendToS("Back");
}

//���� ���������� ��������������
void SClient::userControlMenu()
{
	bool back = 0;
	sendToS("Start user control menu");
	while (!back) {
		system("title ���� ���������� ���������");
		system("cls");
		cout << "���� ���������� ���������\n1 - ������ ��������\n2 - ������� ������ �������\n3 - ������������� ������ �������\n4 - �������� ������\n0 - �����" << endl;
		switch (getMenuChoice(5))
		{
		case 1:
			printUsers();
			system("pause");
			break;
		case 2:
			addUser();
			break;
		case 3:
			editUser();
			break;
		case 4:
			deleteUser();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
}

//���� ���������� ���������
void SClient::productsControlMenu()
{
	bool back = 0;
	sendToS("Start credits control menu");
	while (!back)
	{
		system("cls");
		system("title ���� ���������� ����������");
		cout << "���� ���������� ����������\n1 - ����������� ��������� ����\n2 - �������� ��������� ��� ��������\n3 - ������� ��������� ���\n4 - �������� ���� ��������\n5 - �������� ������\n6 - ������� ������\n0 - �����\n";
		switch (getMenuChoice(7))
		{
		case 1:
			infoGroups();
			break;
		case 2:
			addProdGroup();
			break;
		case 3:
			delProdGroup();
			break;
		case 4:
			printProducts();
			break;
		case 5:
			addProduct();
			break;
		case 6:
			delProduct();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
}



//����� ������
void SClient::printFullReport()
{
	sendToS("Start print full report");
	string result;
	system("cls");
	result = "������ �� ��������� ������ ��������\n\n";
	cout << result;
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		result += "��� ������� �������\n";
		cout << "��� ������� �������\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	result += "\n���������� �� ������������ ��������:\n";
	cout << "\n���������� �� ������������ ��������:\n";
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		cout << "� ���� ��� ��������\n";
		result += "� ���� ��� ��������\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	cout << "������� ��������� ����� � ����?\n1 -  ��\n0 - ���\n";
	if (getMenuChoice(2))
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buf[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buf, 80, "%d-%m-%y %H�%M�", timeinfo);
		system("mkdir ������");
		string filename = "������\\����� �� ";
		filename += buf;
		filename += ".txt";
		fstream file;
		file.open(filename, ios_base::out);
		file << result;
		file.close();
	}
}



//������� ��������
void SClient::printUsers()
{
	sendToS("Start print users");
	User temp;
	int count;
	count = stoi(recvFromS());
	cout << setw(3) << '#' << setw(10) << "���" << setw(10) << "�������" << setw(10) << "�����" << setw(10) << "������" << setw(6) << "����� ���������������?" << endl;
	cout.fill('-');
	cout << setw(49) << '-' << endl;
	cout.fill(' ');
	for (int i = 0; i < count; i++)
	{
		temp.setFName(recvFromS());
		temp.setLName(recvFromS());
		temp.setLogin(recvFromS());
		temp.setPassword(recvFromS());
		temp.setAdminMode(stoi(recvFromS()));
		cout << setw(3) << i+1 << setw(10) << temp.getFName() << setw(10) << temp.getLName() << setw(10) << temp.getLogin() << setw(10) << temp.getPassword() << setw(6) << ((temp.getAdminMode() == true) ? "��" : "���") << endl;
	}
	cout.fill('-');
	cout << setw(49) << '-' << endl;
	cout.fill(' ');
}

//���������� �������
void SClient::addUser()
{
	sendToS("Start add users");
	while (true)
	{
		system("cls");
		system("title ���������� �������");
		string temp;
		cout << "���������� ������ �������\n" << endl;
		cout << "������� �������: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "������� ���: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "������� �����: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		cout << "������� ������: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "�������� ���� ������������\n1 -  �������������\n0 -  ������ (�������)" << endl;
		sendToS(to_string(getMenuChoice(2)));
		serverMsg = recvFromS();
		if (serverMsg == "Successfully added")
			cout << "������ ������� ��������!" << endl;
		else
			if (serverMsg == "Already exist")
				cout << "������������ � ����� ������� � ������� ��� ����������!" << endl;
		cout << "��������� ������?\n1 - ��\n0 - ���" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

//��������� ���������� � �������
void SClient::editUser() 
{
	sendToS("Start edit users");
	while (true)
	{
		printUsers();
		cout << "�������� ������� ��� ��������������" << endl;
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "�� �� ������ �������� ��������� �������!" << endl;
		else
		{

			string temp;
			cout << "������� ����� ������" << endl;
			cout << "������� �������: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "������� ���: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "������� �����: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "������� ������: ";
			rewind(stdin);
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "�������� ���� ������������\n1 - �������������\n0 - ������ (�������)" << endl;
			sendToS(to_string(getMenuChoice(2)));
			serverMsg = recvFromS();
			if (serverMsg == "Already exist")
				cout << "������������ � ����� ������� � ������� ��� ����������!" << endl;
		}
		cout << "��������� ������?\n1 - ��\n0 - ���" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

//�������� ���������� � �������
void SClient::deleteUser()
{
	sendToS("Start delete users");
	while (true)
	{
		printUsers();
		cout << "�������� ������� ��� ��������: ";
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "�� �� ������ ������� ��������� �������!" << endl;
		if (serverMsg == "Successfully deleted")
			cout << "������ ������� �����!" << endl;
		cout << "��������� ������?\n1 - ��\n0 - ���" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}



//����� ���� ����� ��������
void SClient::infoGroups()
{

	sendToS("Start print types");
	while (true) {
		system("cls");
		system("title ��������� ���� ��������");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "�� ������� �� ���� ��� �������!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "�������� ����� ���� �������, ����� ����������� ��������� ����������\n" << endl;
			cout << setw(3) << "#" << setw(20) << "�������� ����" << setw(15) << "������ ������" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - �����" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			if (recvFromS() == "Report ready")
			{
				cout << "��� ������ ����� �������� ����� �����! ������� ��� �����������?\n1 - ��\n0 - ���\n";
				choice = getMenuChoice(2);
				if (choice)
				{
					sendToS("Print report");
					system("cls");
					cout << recvFromS();
				}
				else
					sendToS("Dont print");
			}
			if (choice)
			system("pause");
		}
	}
	sendToS("Back");
}

//���������� ���� �������
void SClient::addProdGroup()
{
	sendToS("Start add types");
	system("cls");
	system("title ���������� ���� �������");
	string temp;
	User tempU;
	bool success = false;
	serverMsg = recvFromS();
	if (serverMsg == "Not ready to add")
	{
		cout << "������! � ���� ������ ���� ������ ������ �������� � ���� �� 1 ������� ��� ������." << endl;
		system("pause");
	}
	else
	{
		cout << "������� �������� ����: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "������� �������� ��������� �����: ";
		sendToS(to_string(getNum<int>()));
		while (true)
		{
			system("cls");
			cout << "�������� ������ ��� ������" << endl;
			cout << setw(3) << "#" << " �������� � id" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS();
				cout << " (id " << recvFromS() << ")" << endl;
			}
			cout << "\n0 - ����������" << endl;
			choice = getMenuChoice(count+1);
			if (!choice)
			{
				sendToS("Next stage");
				break;
			}
			else
				sendToS(to_string(choice - 1));
		}
		while (true)
		{
			system("cls");
			cout << "�������� ������� ��� ������" << endl;
			cout << setw(3) << "#" << " ������� ��� (�����)" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS() << endl;
			}
			cout << "\n0 - ����������" << endl;
			choice = getMenuChoice(count + 1);
			if (!choice)
			{
				sendToS("Next stage");
				break;
			}
			else
				sendToS(to_string(choice - 1));
		}
		cout << recvFromS() << endl;
		system("pause");
	}
	sendToS("Back");
}

//�������� ���� ��������
void SClient::delProdGroup()
{
	sendToS("Start delete types");
	while (true) {
		system("cls");
		system("title ��������� ���� ��������");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "�� ������� �� ���� ���!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "�������� ����� ���� ������� ��� ��������" << endl;
			cout << setw(3) << "#" << setw(20) << "�������� ����" << setw(15) << "������ ������" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - �����" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS() << endl;
			system("pause");
		}
	}
	sendToS("Back");
}

//������� ������ ��������
void SClient::printProducts()
{
	sendToS("Start print credits");
	while (true)
	{
		system("cls");
		system("title ������ ��������");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "�� ������ �� ���� ������!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "�������� ����� �������, ����� ����������� ��������� ����������\n" << endl;
			cout << setw(3) << "#" << setw(40) << "�������� �������" << setw(10) << "ID" << endl;
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(40) << recvFromS();
				cout << setw(10) << recvFromS() << endl;
			}
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			cout << "0 - �����" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			system("pause");
		}

	}
	sendToS("Back");
}

//���������� �������
void SClient::addProduct()
{
	string temp;
	sendToS("Start add credits");
	while (true)
	{
		system("cls");
		system("title ���������� �������");
		cout << "��������� ������ � �������\nID: ";
		temp = to_string(getNum<int>());
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Already exist")
		{
			cout << "������ � ����� ID ��� ����������! �������� ������ ID" << endl;
			system("pause");
		}
		else
		{
			cout << "��������: ";
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "������������ ����� �������: ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			cout << "��������� �� �������: ";
			temp = to_string(getNum <double>());
			sendToS(temp);
			cout << "���� ������ �������: ";
			temp = to_string(getNum<int>());
			sendToS(temp);
			cout << "���������� ������ (��� \'%\'): ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			serverMsg = recvFromS();
			if (serverMsg == "Successfully added")
				cout << "������ ������� ��������!" << endl;
			cout << "��������� ������?\n1 - ��\n0 - ���\n";
			if (getMenuChoice(2))
				break;
		}
	}
	sendToS("Back");
}

//�������� �������
void SClient::delProduct()
{
	sendToS("Start delete credits");

	while (true)
	{
		system("cls");
		system("title ������ ��������");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "�� ������ �� ���� ������!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "�������� ����� ������� ��� ��������" << endl;
			cout << setw(3) << "#" << setw(40) << "�������� �������" << setw(10) << "ID" << endl;
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(40) << recvFromS();
				cout << setw(10) << recvFromS() << endl;
			}
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			cout << "0 - �����" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS() << endl;
			system("pause");
		}

	}

	sendToS("Back");
}



//���������� ������
void SClient::saveData()
{
	sendToS("Save all data");
}