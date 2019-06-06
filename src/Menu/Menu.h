#pragma once
#include <memory>
#include <string>
#include <conio.h>
#include "MenuStateBase.h"

/**
 * \brief Run menu
 * \tparam BaseState First menu state
 * \returns Exit code
 */
template<typename BaseState>
int RunMenu()
{
	static_assert(std::is_base_of<MenuStateBase, BaseState>::value, "BaseState must inherit from MenuStateBase");

	// Push MainMenu into stack
	MenuStack stack;
	stack.push(std::make_unique<BaseState>(&stack));

	while (true)
	{
		// Quit if no menu to show
		if (stack.empty())
			break;

		const auto& current_menu_items = stack.top()->GetMenuItems();
		auto& selected_index = stack.top()->selected_index_;

		// Draw menu
		system("cls");

		const auto title = stack.top()->GetTitle();
		if (!title.empty())
		{
			std::cout << title << std::endl;
			std::cout << std::string(title.length(), '-') << std::endl;
		}

		for (size_t i = 0; i < current_menu_items.size(); i++)
		{
			std::cout << (i == selected_index ? '>' : ' ');
			std::cout << current_menu_items[i];
			std::cout << (i == selected_index ? '<' : ' ');
			std::cout << std::endl;
		}

		// Key handling
		auto key_code = _getch();
		if (key_code == kEnter)  // Enter
		{
			try
			{
				stack.top()->OnSelect();
			}
			catch (std::exception& ex)
			{
				std::cerr << "Error: " << ex.what() << std::endl;
				return 1;
			}
		}
		else if (key_code == kSpecialKeys) // Navigation via arrows
		{
			key_code = _getch();
			if (key_code == kArrowUp) // Up
			{
				if (--selected_index == std::numeric_limits<size_t>::max())
					selected_index = current_menu_items.size() - 1;
			}
			else if (key_code == kArrowDown) // Down
			{
				if (++selected_index >= current_menu_items.size())
					selected_index = 0;
			}
		}
		else if (key_code >= kKey1 && key_code <= kKey9) // Select via numbers
		{
			const int idx = key_code - kKey1;
			if (idx < static_cast<int>(current_menu_items.size()))
			{
				stack.top()->selected_index_ = idx;

				try
				{
					stack.top()->OnSelect();
				}
				catch (std::exception& ex)
				{
					std::cerr << "Error: " << ex.what() << std::endl;
					return 1;
				}
			}
		}
	}

	return 0;
}
