#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <iostream>

/**
 * \brief Menu base class
 */
class Menu
{
public:
	Menu(std::stack<std::unique_ptr<Menu>>* menuStack) : m_menuStack(menuStack)
	{
		selectedIndex = 0;
	}

	virtual ~Menu() = default;

	/**
	 * \brief Method that called when user select the menu
	 */
	virtual void onSelect() = 0;

	/**
	 * \brief Menu items getter
	 */
	[[nodiscard]] const std::vector<std::string>& getMenuItems() const
	{
		return m_menuItems;
	}

	/**
	 * \brief Navigate to menu
	 * 
	 * Navigating to menu class (delivired from Menu) specified in arrows
	 */
	template<class T>
	void navigateTo()
	{
		static_assert(std::is_base_of<Menu, T>::value, "T must inherit from Menu");
		if (m_menuStack == nullptr)
			throw std::exception("m_menuStack is nullptr");

		m_menuStack->push(std::make_unique<T>(m_menuStack));
	}

	/**
	 * \brief Back to previous menu
	 */
	void back()
	{
		if (m_menuStack == nullptr)
			throw std::exception("m_menuStack is nullptr");

		m_menuStack->pop();
	}

	static void pressAnyKey()
	{
		std::cout << "Press any key to continue" << std::endl;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
	}

	static bool dialog()
	{
		std::cout << "Answer (y/n): ";
		char answer;
		std::cin >> answer;

		return tolower(answer) == 'y';
	}

	size_t selectedIndex = 0;

protected:
	std::vector<std::string> m_menuItems;
	std::stack<std::unique_ptr<Menu>>* m_menuStack;
};
