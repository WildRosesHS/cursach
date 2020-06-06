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
	system("title О программе");
	cout << "Курсовой проект по дисциплине САиПИС Ошуркевича Павла Николаевича\nТема: \"Программное средство для принятия решений методом полного попарного сравнения (на примере кредитной организации)\"\nМинск, 2020" << endl;
	system("pause");
}

int SClient::getMenuChoice(int countOfMenuBoxes, int numberOfFirst)
{
	int choice;
	cout << "Введите ваш выбор - ";
	while (!(cin >> choice) || choice < numberOfFirst || choice > (numberOfFirst + countOfMenuBoxes - 1))
	{
		cout << "Неправильный набор! Повторите попытку - ";
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
		cout << "Неправильный набор! Повторите попытку - ";
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
		cout << "Не удалось соединиться с сервером, проверьте работоспособность сервера и повторите попытку" << endl;
		system("pause");
	}
	try {
		startMenu();
	}
	catch (int)
	{
		cout << "Сервер оборвал соединение.\n";
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

//стартовое меню
void SClient::startMenu()
{
	bool back = 0;
	while (!back)
	{
		system("cls");
		system("title Стартовое меню");
		cout << "Вас приветствуеет ассистент банка \"Альфа\"" << endl;
		cout << "1 - Авторизация клиента\n2 - Регистрация клиента\n0 - Выход из программы\n";
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



//авторизация
void SClient::auth()
{	
	bool back = 0;
	sendToS("Start authorization");
	while (!back) {
		system("cls");
		system("title Авторизация");
		string temp;
		cout << "Авторизация:\n" << endl;
		cout << "Логин: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		currentUser.setLogin(temp);
		sendToS(temp);
		cout << "Пароль: ";
		getline(cin, temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Error: user not founded")
		{
			cout << "Неправильно введен логин или пароль. Желаете продолжить?\n1 - да\n0 - нет\n";
			if (!getMenuChoice(2))
				back = true;
		}
		else
		{
			currentUser.setAdminMode(stoi(recvFromS()));
			currentUser.setFName(recvFromS());
			currentUser.setLName(recvFromS());
			cout << "Вы были авторизированы." << endl;
			if (currentUser.getFName() != "")
				cout << "Добро пожаловать в онлайн отделение банка, " << currentUser.getFName() << "!\n";
			system("pause");
			if (currentUser.getAdminMode())
				back = adminMenu();
			else
				back = userMenu();
		}
	}
	sendToS("Back");
}



//регистрация пользователя
void SClient::registration()
{
	sendToS("Start registration");
	while (true)
	{
		system("title Регистрация");
		system("cls");
		string temp;
		cout << "Введите фамилию: ";
		cin >> temp;
		currentUser.setLName(temp);
		sendToS(temp);
		cout << "Введите имя: ";
		cin >> temp;
		sendToS(temp);
		currentUser.setFName(temp);
		cout << "Введите логин: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setLogin(temp);
		cout << "Введите пароль: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		serverMsg = recvFromS();
		if (serverMsg == "Already exist")
		{
			cout << "Пользователь с таким логином и паролем уже существует!\nПовторить попытку?\n1 - Да\n0 - нет" << endl;
			if (!getMenuChoice(2))
				break;
		}
		if (serverMsg == "Successful registration")
		{
			cout << "Вы были зарегистрированы." << endl;
			system("pause");
			break;
		}
	}
	sendToS("Back");
}



//меню администратора
bool SClient::adminMenu()
{
	bool back = false;
	sendToS("Start admin menu");
	while (!back)
	{
		system("title Меню администратора");
		system("cls");
		cout << "Администратор: " << currentUser.getFName() << "\n1 - Управление данными клиентов\n2 - Редактирование данных о кредитах\n3 - Отчет о экспертных оценках\n4 - Издатель\n5 - Сохранения данных\n0 - Выйти\n";
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

//меню пользователя
bool SClient::userMenu()
{
	bool back = false;
	sendToS("Start user menu");
	while (!back)
	{
		system("cls");
		system("title Меню пользователя");
		cout << "Приветствуем Вас, " << currentUser.getFName() << "\n1 - Кредиты для оценки\n2 - Список всех кредитов\n3 - Список всех типов кредитов\n4 - Создать отчет\n5 - Издатель\n0 - Выйти" << endl;
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

//меню экспертных оценок
void SClient::expertRateMenu()
{
	sendToS("Start expert rate menu");
	while (true) {
		system("cls");
		system("title Оценка типов кредита");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найден ни один кредит!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер типа кредита, чтобы просмотреть информацию о нем" << endl;
			cout << setw(3) << "#" << setw(20) << "Название типа кредита" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - Выйти" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			cout << "Начать оценку типа кредита?\n1 - Да\n0 - Нет" << endl;
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

//меню управления пользователями
void SClient::userControlMenu()
{
	bool back = 0;
	sendToS("Start user control menu");
	while (!back) {
		system("title Меню управления клиентами");
		system("cls");
		cout << "Меню управления клиентами\n1 - Список клиентов\n2 - Создать нового клиента\n3 - Редактировать данные клиента\n4 - Удаление данных\n0 - Выйти" << endl;
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

//меню управления кредитами
void SClient::productsControlMenu()
{
	bool back = 0;
	sendToS("Start credits control menu");
	while (!back)
	{
		system("cls");
		system("title Меню управления крредитами");
		cout << "Меню управления кредитамми\n1 - Просмотреть оценочные типы\n2 - Добавить оценочный тип кредитов\n3 - Удалить оценочный тип\n4 - Просмотр всех кредитов\n5 - Добавить кредит\n6 - Удалить кредит\n0 - Выйти\n";
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



//вывод отчета
void SClient::printFullReport()
{
	sendToS("Start print full report");
	string result;
	system("cls");
	result = "Отчеты по оценочным типамм кредитов\n\n";
	cout << result;
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		result += "Нет готовых отчётов\n";
		cout << "Нет готовых отчётов\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	result += "\nИнформация по существующим кредитам:\n";
	cout << "\nИнформация по существующим кредитам:\n";
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		cout << "В базе нет кредитов\n";
		result += "В базе нет кредитов\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	cout << "Желаете сохранить отчёт в файл?\n1 -  Да\n0 - Нет\n";
	if (getMenuChoice(2))
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buf[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buf, 80, "%d-%m-%y %Hч%Mм", timeinfo);
		system("mkdir Отчёты");
		string filename = "Отчёты\\Отчёт на ";
		filename += buf;
		filename += ".txt";
		fstream file;
		file.open(filename, ios_base::out);
		file << result;
		file.close();
	}
}



//вывести клиентов
void SClient::printUsers()
{
	sendToS("Start print users");
	User temp;
	int count;
	count = stoi(recvFromS());
	cout << setw(3) << '#' << setw(10) << "Имя" << setw(10) << "Фамилия" << setw(10) << "Логин" << setw(10) << "Пароль" << setw(6) << "Права адмминистратора?" << endl;
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
		cout << setw(3) << i+1 << setw(10) << temp.getFName() << setw(10) << temp.getLName() << setw(10) << temp.getLogin() << setw(10) << temp.getPassword() << setw(6) << ((temp.getAdminMode() == true) ? "Да" : "Нет") << endl;
	}
	cout.fill('-');
	cout << setw(49) << '-' << endl;
	cout.fill(' ');
}

//добавление клиента
void SClient::addUser()
{
	sendToS("Start add users");
	while (true)
	{
		system("cls");
		system("title Добавление клиента");
		string temp;
		cout << "Добавление нового клиента\n" << endl;
		cout << "Введите фамилию: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "Введите имя: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "Введите логин: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		cout << "Введите пароль: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "Выберите роль пользователя\n1 -  Администратор\n0 -  Клиент (эксперт)" << endl;
		sendToS(to_string(getMenuChoice(2)));
		serverMsg = recvFromS();
		if (serverMsg == "Successfully added")
			cout << "Клиент успешно добавлен!" << endl;
		else
			if (serverMsg == "Already exist")
				cout << "Пользователь с таким логином и паролем уже существует!" << endl;
		cout << "Закончить работу?\n1 - Да\n0 - Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

//изменение информации о клиенте
void SClient::editUser() 
{
	sendToS("Start edit users");
	while (true)
	{
		printUsers();
		cout << "Выберите клиента для редактирования" << endl;
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "Вы не можете изменять активного клиента!" << endl;
		else
		{

			string temp;
			cout << "Введите новые данные" << endl;
			cout << "Введите фамилию: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите имя: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите логин: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите пароль: ";
			rewind(stdin);
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "Выберите роль пользователя\n1 - Администратор\n0 - Клиент (эксперт)" << endl;
			sendToS(to_string(getMenuChoice(2)));
			serverMsg = recvFromS();
			if (serverMsg == "Already exist")
				cout << "Пользователь с таким логином и паролем уже существует!" << endl;
		}
		cout << "Закончить работу?\n1 - Да\n0 - Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

//удаление информации о клиенте
void SClient::deleteUser()
{
	sendToS("Start delete users");
	while (true)
	{
		printUsers();
		cout << "Выберите клиента для удаления: ";
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "Вы не можете удалить активного клиента!" << endl;
		if (serverMsg == "Successfully deleted")
			cout << "Клиент успешно удалён!" << endl;
		cout << "Закончить работу?\n1 - Да\n0 - Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}



//вывод всех типов кредитов
void SClient::infoGroups()
{

	sendToS("Start print types");
	while (true) {
		system("cls");
		system("title Оценочные типы кредитов");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найдена ни один тип кредита!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер типа кредита, чтобы просмотреть подробную информацию\n" << endl;
			cout << setw(3) << "#" << setw(20) << "Название типа" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - Назад" << endl;
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
				cout << "Для данных типов кредитов готов отчёт! Желаете его просмотреть?\n1 - Да\n0 - Нет\n";
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

//добавление типа кредита
void SClient::addProdGroup()
{
	sendToS("Start add types");
	system("cls");
	system("title Добавление типа кредита");
	string temp;
	User tempU;
	bool success = false;
	serverMsg = recvFromS();
	if (serverMsg == "Not ready to add")
	{
		cout << "Ошибка! В базе должно быть больше одного продукта и хотя бы 1 эксперт для оценки." << endl;
		system("pause");
	}
	else
	{
		cout << "Введите название типа: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "Введите маскимум оценочной шкалы: ";
		sendToS(to_string(getNum<int>()));
		while (true)
		{
			system("cls");
			cout << "Выберите кредит для оценки" << endl;
			cout << setw(3) << "#" << " Название и id" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS();
				cout << " (id " << recvFromS() << ")" << endl;
			}
			cout << "\n0 - Продолжить" << endl;
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
			cout << "Выберите клиента для оценки" << endl;
			cout << setw(3) << "#" << " Фамилия Имя (логин)" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS() << endl;
			}
			cout << "\n0 - Продолжить" << endl;
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

//удаление типа кредитов
void SClient::delProdGroup()
{
	sendToS("Start delete types");
	while (true) {
		system("cls");
		system("title Оценочные типы кредитов");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найдена ни один тип!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер типа кредита для удаления" << endl;
			cout << setw(3) << "#" << setw(20) << "Название типа" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0 - Выход" << endl;
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

//вывести список кредитов
void SClient::printProducts()
{
	sendToS("Start print credits");
	while (true)
	{
		system("cls");
		system("title Список кредитов");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найден ни один кредит!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер кредита, чтобы просмотреть подробную информацию\n" << endl;
			cout << setw(3) << "#" << setw(40) << "Название кредита" << setw(10) << "ID" << endl;
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
			cout << "0 - Назад" << endl;
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

//добавление кредита
void SClient::addProduct()
{
	string temp;
	sendToS("Start add credits");
	while (true)
	{
		system("cls");
		system("title Добавление кредита");
		cout << "Заполните данных о кредите\nID: ";
		temp = to_string(getNum<int>());
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Already exist")
		{
			cout << "Кредит с таким ID уже существует! Выберите другой ID" << endl;
			system("pause");
		}
		else
		{
			cout << "Название: ";
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "Максимальная сумма кредита: ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			cout << "Переплата по кредиту: ";
			temp = to_string(getNum <double>());
			sendToS(temp);
			cout << "Срок выдачи кредита: ";
			temp = to_string(getNum<int>());
			sendToS(temp);
			cout << "Процентная ставка (без \'%\'): ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			serverMsg = recvFromS();
			if (serverMsg == "Successfully added")
				cout << "Кредит успешно добавлен!" << endl;
			cout << "Завершить работу?\n1 - Да\n0 - Нет\n";
			if (getMenuChoice(2))
				break;
		}
	}
	sendToS("Back");
}

//удаление кредита
void SClient::delProduct()
{
	sendToS("Start delete credits");

	while (true)
	{
		system("cls");
		system("title Список кредитов");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найден ни один кредит!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер кредита для удаления" << endl;
			cout << setw(3) << "#" << setw(40) << "Название кредита" << setw(10) << "ID" << endl;
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
			cout << "0 - Назад" << endl;
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



//сохранение данных
void SClient::saveData()
{
	sendToS("Save all data");
}