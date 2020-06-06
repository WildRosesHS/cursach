#pragma once
#include "Product.h"
#include "User.h"
class ProductGroup
{
	bool readyForReport = false;
	int maxOfScale;
	string name;
	map<Product,double>  products; // ����� - �������� ������, �� ���� ��������� ��� ������� �������� �������������� (����� ��������������� ����� � �������� ������)
	map<User, vector<double>> experts; // ������ ���� ���� �� ������� �������� ��� ������� �������� �������������� ��������
public:
	void checkReady();
	bool isReady() { return readyForReport; }
	string getName() { return name; }
	string getExpertsForPrint ();
	string getProductsForPrint();
	string getReport();
	int getMaxOfScale() { return maxOfScale; }
	map<Product, double> getProducts() { return products; }
	map<User, vector<double>> getExperts() { return experts; }
	bool operator == (ProductGroup p) const {
		return (name == p.name);
	}
	ProductGroup(string name, int scale, vector <User>, vector <Product>);
	ProductGroup(string name, int scale, map <User, vector<double>>, vector <Product>);
	~ProductGroup();
	friend class ClientInterface;
	friend class opDataBase;
};

