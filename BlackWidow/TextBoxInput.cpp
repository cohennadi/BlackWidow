#include "TextBoxInput.h"

std::string G_text_input;

TextBoxInput::TextBoxInput(HINSTANCE instance_handle, const std::string& window_class_name, const std::string& title)
{
	register_class(instance_handle, window_class_name);
	create_window(instance_handle, window_class_name, title);
}

void TextBoxInput::window_loop() const
{
	constexpr UINT ALL_AVAILABLE_MESSAGES = 0;
	constexpr HWND MSGS_FROM_ANY_WINDOW_BELONGS_TO_CURRENT_THREAD = nullptr;

	MSG messages;
	while (GetMessage(&messages, MSGS_FROM_ANY_WINDOW_BELONGS_TO_CURRENT_THREAD, ALL_AVAILABLE_MESSAGES, ALL_AVAILABLE_MESSAGES))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
}

std::string TextBoxInput::text_input() const
{
	return G_text_input;
}

void TextBoxInput::register_class(HINSTANCE instance_handle, const std::string& window_class_name)
{
	constexpr LPCSTR NO_DEFUALT_MENU = nullptr;
	constexpr int NO_EXTRA_CLASS_MEMORY = 0;
	constexpr int NO_EXTRA_WINDOW_MEMORY = 0;

	WNDCLASSEXA window_class;

	window_class.hInstance = instance_handle;
	window_class.lpszClassName = window_class_name.c_str();
	window_class.lpfnWndProc = window_logic;
	window_class.style = CS_DBLCLKS;
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	window_class.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class.lpszMenuName = NO_DEFUALT_MENU;
	window_class.cbClsExtra = NO_EXTRA_CLASS_MEMORY;
	window_class.cbWndExtra = NO_EXTRA_WINDOW_MEMORY;
	window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

	if (!RegisterClassExA(&window_class))
	{
		//todo: raise exception 
	}
}

void TextBoxInput::create_window(HINSTANCE instance_handle, const std::string& window_class_name, const std::string& title)
{
	constexpr DWORD NO_EXTENDED_WINDOW_STYLE = 0;
	constexpr int DEFAULT_X_POSITION = CW_USEDEFAULT;
	constexpr int DEFAULT_Y_POSITION = CW_USEDEFAULT;
	constexpr int WIDTH = 400;
	constexpr int HEIGHT = 200;
	constexpr HMENU NO_MENU = nullptr;
	constexpr LPVOID NO_CREATION_PARAMS = nullptr;
	constexpr int SHOW_WINDOW = SW_SHOW;

	const HWND window_handle = CreateWindowExA(NO_EXTENDED_WINDOW_STYLE,
											   window_class_name.c_str(),
											   title.c_str(),
											   WS_OVERLAPPEDWINDOW,
											   DEFAULT_X_POSITION,
											   DEFAULT_Y_POSITION,
											   WIDTH,
											   HEIGHT,
											   HWND_DESKTOP,
											   NO_MENU,
											   instance_handle,
											   NO_CREATION_PARAMS);

	if (!window_handle)
	{
		// todo: raise exception
	}

	ShowWindow(window_handle, SHOW_WINDOW);
	UpdateWindow(window_handle);
}

enum MenuOptions
{
	EDIT_TEXT_BOX = 1000,
	ENTER_KEY = 1001
};

LRESULT TextBoxInput::window_logic(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	constexpr int EDIT_X = 5;
	constexpr int EDIT_Y = 5;
	constexpr int EDIT_WIDTH = 250;
	constexpr int EDIT_HEIGHT = 20;
	constexpr int INSERT_X = EDIT_X + EDIT_WIDTH + 10;
	constexpr int INSERT_Y = EDIT_Y;
	constexpr int INSERT_WIDTH = 100;
	constexpr int INSERT_HEIGHT = EDIT_HEIGHT;
	constexpr LPVOID NO_CREATION_PARAMS = nullptr;
	constexpr LPCSTR NO_TITLE = nullptr;
	constexpr int MAX_COUNT = 100;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		CreateWindowA("edit",
					  NO_TITLE,
					  WS_CHILD | WS_VISIBLE | WS_BORDER,
					  EDIT_X,
					  EDIT_Y,
					  EDIT_WIDTH,
					  EDIT_HEIGHT,
					  hwnd,
					  reinterpret_cast<HMENU>(EDIT_TEXT_BOX),
					  GetModuleHandle(NULL),
					  NO_CREATION_PARAMS);
		CreateWindowA("button",
					  "Insert",
					  WS_CHILD | WS_VISIBLE,
					  INSERT_X,
					  INSERT_Y,
					  INSERT_WIDTH,
					  INSERT_HEIGHT,
					  hwnd,
					  reinterpret_cast<HMENU>(ENTER_KEY),
					  GetModuleHandle(NULL),
					  NO_CREATION_PARAMS);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ENTER_KEY:
		{
			std::string input;
			input.resize(MAX_COUNT);
			GetWindowTextA(GetDlgItem(hwnd, EDIT_TEXT_BOX), const_cast<char*>(input.c_str()), MAX_COUNT);
			G_text_input = std::move(input);
			PostQuitMessage(0);
		}
		break;
		}

		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
