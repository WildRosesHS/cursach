#include "Product.h"



Product::Product()
{
	name = "";
	price = 0.0;
	storageCost = 0.0;
	sellByDays = 0;
	nonRealizedPercent = 0;
}

Product::Product(int ID)
{
	productID = ID;
	name = "";
	price = 0.0;
	storageCost = 0.0;
	sellByDays = 0;
	nonRealizedPercent = 0;
}

string Product::getFullInfo() const
{
	string result;
	result += "�������� �������: " + getName() + "\nID: " + to_string(getID()) + "\n������������ �����: " + cutStrDouble(commaToDot(to_string(getPrice()))) + " ���.\n��������� �� �������: " + cutStrDouble(commaToDot(to_string(getStorageCost())));
	result += " ���.\n���� ������ �������: " + to_string(getSellByDays()) + " ����\n������� ��������: " + cutStrDouble(commaToDot(to_string(getNonRealizedPercent()))) + "%\n";
	return result;
}

Product::~Product()
{
}
