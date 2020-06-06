#pragma once
#include <vector>
#include "ProductGroup.h"
#include <fstream>
using namespace std;
class opDataBase
{
	opDataBase();
	vector<User> userVector;// ��� ������������
	vector<User> activeUsers;// �������� ������������
	vector<Product> productVector;//��� ��������
	vector<ProductGroup> prodGroupVector;//��� ������ �������
public:
	static opDataBase& Hinstance()
	{
		static opDataBase Obj;
		return Obj;
	}
	void saveAll();	
	void saveUsers ();
	void saveProducts();
	void saveProductGroups();
	void uploadUsers();
	void uploadProducts();
	void uploadProductGroups();
	void uploadAll();
	~opDataBase();
	friend class ClientInterface;
};

