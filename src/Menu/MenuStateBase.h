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
	kEnter = 13,
	kEscape = 27,
	kSpace = 32,

	kSpecialKeys = 224,
	kArrowUp = 72,
	kArrowDown = 80,

	kKey1 = 49,
	kKey2,
	kKey3,
	kKey4,
	kKey5,
	kKey6,
	kKey7,
	kKey8,
	kKey9,
	kKey0
};

class MenuStateBase;
typedef std::stack<std::unique_ptr<MenuStateBase>> MenuStack;

/**
 * \brief Menu base class
 */
class MenuStateBase
{
public:
	explicit MenuStateBase(MenuStack* menu_stack) : menu_stack_(menu_stack)
	{
		selected_index_ = 0;
	}

	virtual ~MenuStateBase() = default;

	/**
	 * \brief Method that called when user select the menu
	 */
	virtual void OnSelect() = 0;
	
	[[nodiscard]] const std::string& GetTitle() const
	{
		return title_;
	}

	[[nodiscard]] const std::vector<std::string>& GetMenuItems() const
	{
		return menu_items_;
	}

	/**
	 * \brief Navigate to menu
	 *
	 * Navigating to menu class (delivered from Menu) specified in arrows
	 */
	template <class T>
	void NavigateTo()
	{
		static_assert(std::is_base_of<MenuStateBase, T>::value, "T must inherit from Menu");
		if (menu_stack_ == nullptr)
			throw std::exception("menu_stack_ is nullptr");

		menu_stack_->push(std::make_unique<T>(menu_stack_));
	}

	/**
	 * \brief Back to previous menu
	 */
	void Back() const
	{
		if (menu_stack_ == nullptr)
			throw std::exception("menu_stack_ is nullptr");

		menu_stack_->pop();
	}

	/**
	 * \brief Wait for escape/space/enter press
	 */
	static void WaitForEscape()
	{
		std::cout << "Press escape to continue" << std::endl;
		int key_code;
		do key_code = _getch();
		while(key_code != kEscape && key_code != kSpace && key_code != kEnter);
	}

	/**
	 * \brief Wait for user answering
	 */
	static bool Ask(std::string_view question)
	{
		std::cout << question << std::endl
			<< "Answer (y/n): ";
		char answer;
		std::cin >> answer;

		return tolower(answer) == 'y';
	}

	/**
	 * \brief Read value of specified type
	 * \param predicate If specified, value also check with this predicate
	 */
	template <typename T>
	T ReadValue(std::function<bool(T)> predicate = nullptr) const
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

	size_t selected_index_ = 0;

protected:
	std::string title_;
	std::vector<std::string> menu_items_;
	MenuStack* menu_stack_;
};
