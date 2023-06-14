#include<iostream>
#include <string>

std::string	ft_tostring(int nbr)
{
	std::string	str;

	while (nbr > 10)
	{
		str.insert(0,1, static_cast<char>((nbr % 10) + '0'));
		nbr /= 10;
	}
	if (nbr > 0)
		str.insert(0,1, static_cast<char>(nbr+ '0'));
	return (str);
}

int main()
{
	int nbr = 1202548;
	std::cout << "string = " << ft_tostring (nbr) << std::endl;
}