#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <conio.h>

enum KeyCode
{
	Enter = 13,
	Escape = 27,

	SpecialKeys = 224,
	ArrowUp = 72,
	ArrowDown = 80,

	Key1 = 49,
	Key9 = 57
};

/**
 * \brief Menu base class
 */
class MenuBase
{
public:
	explicit MenuBase(std::stack<std::unique_ptr<MenuBase>>* menuStack) : m_menuStack(menuStack)
	{
		selectedIndex = 0;
	}

	virtual ~MenuBase() = default;

	/**
	 * \brief Method that called when user select the menu
	 */
	virtual void onSelect() = 0;
	
	[[nodiscard]] const std::string& getTitle() const
	{
		return m_title;
	}

	[[nodiscard]] const std::vector<std::string>& getMenuItems() const
	{
		return m_menuItems;
	}

	/**
	 * \brief Navigate to menu
	 *
	 * Navigating to menu class (delivered from Menu) specified in arrows
	 */
	template <class T>
	void navigateTo()
	{
		static_assert(std::is_base_of<MenuBase, T>::value, "T must inherit from Menu");
		if (m_menuStack == nullptr)
			throw std::exception("m_menuStack is nullptr");

		m_menuStack->push(std::make_unique<T>(m_menuStack));
	}

	/**
	 * \brief Back to previous menu
	 */
	void back() const
	{
		if (m_menuStack == nullptr)
			throw std::exception("m_menuStack is nullptr");

		m_menuStack->pop();
	}

	/**
	 * \brief Wait for escape press
	 */
	static void waitForEscape()
	{
		std::cout << "Press escape to continue" << std::endl;
		while(_getch() != Escape);
	}

	/**
	 * \brief Wait for user answering
	 */
	static bool dialog()
	{
		std::cout << "Answer (y/n): ";
		char answer;
		std::cin >> answer;

		return tolower(answer) == 'y';
	}

	/**
	 * \brief Read value of specified type
	 * \param predicate If specified, value also check with this predicate
	 */
	template <typename T>
	T readValue(std::function<bool(T)> predicate = nullptr) const
	{
		T value;
		while (!(std::cin >> value) ||
			(predicate != nullptr && !predicate(value)))
		{
			std::cout << "Incorrect value. Try again: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		return value;
	}

	size_t selectedIndex = 0;

protected:
	std::string m_title;
	std::vector<std::string> m_menuItems;
	std::stack<std::unique_ptr<MenuBase>>* m_menuStack;
};
