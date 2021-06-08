#pragma once


#include <string>
#include <Windows.h>

class TextBoxInput final
{
public:

	// ctor
	// param[in]	instance_handle			handle to application instance.
	// param[in]	window_class_name		name of window class.
	// param[in]	title					the window title.
	TextBoxInput(HINSTANCE instance_handle, const std::string& window_class_name, const std::string& title);

	// dtor
	~TextBoxInput() = default;

	// Executes the window loop.
	void window_loop() const;

	// The text input.
	// return								the text input.
	[[nodiscard]] std::string text_input() const;

	// Deleted functions
	TextBoxInput(const TextBoxInput&) = delete;
	TextBoxInput(TextBoxInput&&) = delete;
	TextBoxInput& operator=(const TextBoxInput&) = delete;
	TextBoxInput& operator=(TextBoxInput&&) = delete;

private:

	// Register window class.
	// param[in]	instance_handle			handle to application instance.
	// param[in]	window_class_name		name of window class.
	static void register_class(HINSTANCE instance_handle, const std::string& window_class_name);

	// Create window.
	// param[in]	instance_handle			handle to application instance.
	// param[in]	window_class_name		name of window class.
	// param[in]	title					the window title.
	static void create_window(HINSTANCE instance_handle, const std::string& window_class_name, const std::string& title);

	// Callback for register class.
	static LRESULT CALLBACK window_logic(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
