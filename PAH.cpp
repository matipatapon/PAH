// PAH.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <fstream>
#include <chrono>
#include <ole2.h>
#include <olectl.h>
using namespace std;

char * dec_to_hex(int dec, const char * wynik = "")
{
   
    //Needed characters
    char table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C','D', 'E', 'F'};
    int r = dec % 16;
    dec = dec / 16;
    //cout << "Reszta : " << r<<endl;
    wynik += table[r];
    if(dec!=0)
    {
        return dec_to_hex(dec, wynik);
    }
    //Reversing string ! 
    char* buffer = {};
    //cout << "Wynik = " << wynik << " sizeof : " << wynik.size() << endl;
    for(int i = strlen(wynik)-1;i>=0;i--)
    {
        buffer += wynik[i];
    }
    return buffer;
    
}
int hex_to_dec(int hex)
{
    return hex;
}
class pixel{
    char* red = new char[2];
    char* green = new char[2];
    char* blue = new char[2];
    //Get string 
public:
    pixel(string hex){
        int i = 0;
        int c = 0;
        if(hex.length() == 5)
        {
            red[0] = '0';
            cout << "Uzupełniono 0 w redzie !" << endl;
        }
        else if(hex.length()!=6)
        {
            cout << "Nieobsługiwana długość argumentu ! " << endl;
            return;
        }
        
        this->red[0] = hex[0];
        this->red[1] = hex[1];
        this->green[0] = hex[2];
        this->green[1] = hex[3];
        this->blue[0] = hex[4];
        this->blue[1] = hex[5];
        
    };

};

class pah
{
protected:
    int screen_width;
    int screen_height;
    HDC hdc;
    
    const char* filename = "save.txt\0";

    void load()
    {
        string header;
        fstream Fily(filename,ios::in);
        while (Fily >> header) {

            if (header == "Pin") {
                Fily >> pin;
            }
            if (header == "Special_Key") {
                Fily >> special_key;
            }
            
            
        }
        Fily.close();
    }
    void save()
    {
        fstream Fily(filename, ios::out);
        Fily << "Pin\n" << pin;
        Fily.close();
        
    }

  
    char* read_kb() {
        char* keys = new char[26];
        ZeroMemory(keys, sizeof(char) * 26);
        int index = 0;

        for (int i = 0x30; i <= 0x5A; i++)
        {
            //printf("Checking key : %c\n", char(i));
            if (GetAsyncKeyState(i) == -32767)
            {
                //cout << "klik " << char(i) << endl;
                keys[index] = char(i);
                index++;
            }        
        }
        return keys;
        
    }
    void wait_after_press(int extra = 0)
    {
        Sleep(rand() % 20 + 100 + extra);
    }
    //Go forward ? 
    bool forward = false;
    void walk_forward(bool state)
    {
        INPUT input[1] = {};
        input[0].type = INPUT_KEYBOARD;
        if(!state)
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        input[0].ki.wVk = 0x57;
        unsigned int wykonano = SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
    }
    /*This function will emulate keyboard and inject specified pin to Ark or whatever*/
    int pin = 0;
    bool injecting = false;
    //wait after giving pin for x frames !
    double wait_def = 20;
    double wait = 0;
    bool pin_injected = false;
    bool pin_inject()
    {
            //Injecting
            pin_injected = false;
            INPUT input[2] = {};
            input[1].type = input[0].type = INPUT_KEYBOARD;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;
            input[1].ki.wVk = input[0].ki.wVk = 0x45;
            unsigned int wykonano = SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
            wait_after_press(500);
           
            //if pin panel isn't displayed stop ! 
            pin_panel_color_check = GetPixel(hdc, (*pin_panel_point).x, (*pin_panel_point).y);
            if (pin_panel_color != pin_panel_color_check) {
                return false;
            }
            
            if (wykonano != 2)
            {
                return false;
            }
            wait = wait_def;
            if (pin > 9999)
            {
                pin = 0;
            }

            int rpin = pin;
            int npin = 0;
            int p = 1000;
            //Reversing pin 
            for (int i = 0; i < 4; i++)
            {
                int r = rpin % 10;
                rpin = (rpin - r) / 10;
                npin += r * p;
                p /= 10;
            }
            rpin = npin;
            for (int i = 0; i < 4; i++)
            {
                int r = rpin % 10;
                rpin = (rpin - r) / 10;
                input[1].ki.wVk = input[0].ki.wVk = 0x60 + r;

                unsigned int wykonano = SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
                wait_after_press();
                if (wykonano != 2)
                {
                    return false;
                }



            }
            //input[1].ki.wVk = input[0].ki.wVk = VK_RETURN;
            //wykonano = SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
            //Checking color !

            pin++;
            pin_injected = true;
            return true;
        
    }
    POINT* point = new POINT;
    int color = 0;
    int color_check = 0;
    POINT* pin_panel_point = new POINT;
    int pin_panel_color = 0;
    int pin_panel_color_check = 0;
    //Remember where mouse is !!!
    int set_color(string type) {
        POINT** x;
        if(type == "pin")
            x = &pin_panel_point;
        else if (type == "color")
            x = &point;
        else return 0;
        GetCursorPos(*x);
        return GetPixel(hdc, (**x).x, (**x).y);
        screen();
    }
    void char_pin(char * x)
    {
        
        char table[10] = { '0','1','2','3','4','5','6','7','8','9' };
        //in pin variable next pin is stored !
        int tpin = pin-1;
        for (int i = 0; i < 4; i++)
        {
            int r = tpin % 10;
            tpin = (tpin - r) / 10;
            x[3 - i] = table[r];
        }
        x[4] = '\0';
       
    }
    //Output everything !
    char result[5] = { 'N','O','N','E','\0' };
    const char * get_storage_type()
    {
        switch(object)
        {
        case gate:
            return "GATE";
        case storage:
            return "STORAGE";
        default:
            return "NONE";
        }
            }
    void screen()
    {

        const char* injection = (injecting) ? "TRUE" : "FALSE";
        const char* forw = (forward) ? "TRUE" : "FALSE";
        char cpin[5] = {};
        char* ppin = &cpin[0];
       
        char_pin(ppin);
        
        const char* object_type = get_storage_type();
        
       
        
        const char * output = 
        "PAH V0.1\n"
        "Object type : %s\n"
        "NEXT PIN IN : %f\n"
        "Cursor locked on : %ix|%iy\n"
        "Waiting for color : %06X\n"
        "Current color : %06X\n"
        "Pin panel lock on : %ix|%iy\n"
        "Waiting for color : %06X\n"
        "Current color : %06X\n"
        "Injecting = %s\n"
        "Go forward ? = %s\n"
        "Current pin = %s\n"
        "Result = %s\n"
        "\n"
        
        ;
        system("cls");
        printf(output,object_type,wait,(*point).x,(*point).y,color,color_check,(*pin_panel_point).x,(*pin_panel_point).y,pin_panel_color,pin_panel_color_check,injection, forw ,cpin,result);
        
        
    }

    //Key to trigger action ! 
    int special_key = VK_SCROLL;
    bool Exit = false;
    enum hacking_object { gate = 0, storage, hacking_object_length };
    int object = gate;
    int g_s_o_c_size = 50;

    //Get pixel wchich are nearby locked pixel like : 
    void get_square_of_colors(int* table, int px, int py)
    {
        int element = 0;
        int offset = sqrt(g_s_o_c_size);
        for (int x = offset / -2; x <= offset / 2; x += 1)
        {
            for (int y = offset / -2; y <= offset / 2; y += 1) {

                int colorek = GetPixel(hdc, px + x, py + y);
                table[element++] = colorek;

                if (element >= g_s_o_c_size)
                {
                    return;
                }

            }

        }
    }

    bool color_check_square(int* table, int color)
    {
        const int size = g_s_o_c_size;
        for (int i = 0; i < size; i++)
        {
            if (table[i] == color) {
                return true;
            }
        }
        return false;
    }

    double elapsed = 0;
  
public:
    
    bool print_screen()
    {
        
        //Making print screen 
        INPUT key[2] = {};
        key[0].ki.wVk = key[1].ki.wVk = VK_SNAPSHOT;
        key[0].type = key[1].type = INPUT_KEYBOARD;
        key[1].ki.dwFlags = KEYEVENTF_KEYUP;
        int x = SendInput(2, key, sizeof(INPUT));
        printf("\nWykonano %i\n", x);
        //Locking clipboard !
        OpenClipboard(NULL);
        HBITMAP content = (HBITMAP)GetClipboardData(CF_DIB);
        CloseClipboard();
        //https://stackoverflow.com/questions/9524393/how-to-capture-part-of-the-screen-and-save-it-to-a-bmp
        
        //Creating bitmap !!! yey ! 
        /*
        Bitmap structure : 


        BITMAPFILEHEADER 
            bfType <- the file type must be BM
            bfSize <- The size in bytes of the bitmap file
            bfReserved1 <- muste be 0
            bfReserved2 <- must be 0
            bfOffBits <- The offset in bytes from the beginnig of the BITMAPFILEHEADER stucture to the bitmap bits

        BITMAPINFOHEADER 
            https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)

        RGBQUAD array 

        Color-index array 
        */
        //https://stackoverflow.com/questions/18838553/c-how-to-create-a-bitmap-file
        int Width = screen_width;
        int Height = screen_height;

        //Making sure width is divisible by four
        while(Width % 4 != 0)
        {
            Width++;
        }

        BITMAPFILEHEADER bfh;
        bfh.bfType = 0x4d42;
        bfh.bfSize = 14 + 40 + Width*Height * 3;
        bfh.bfReserved1 = 0;
        bfh.bfReserved2 = 0;
        bfh.bfOffBits = 14 + 40;

        BITMAPINFOHEADER bih;
        memset(&bih, 0, 40);
        bih.biSize = 40;
        bih.biWidth = Width;
        bih.biHeight = Height;
        bih.biPlanes = 1;
        bih.biBitCount = 24;
        bih.biCompression = BI_RGB;

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = screen_width;
        bmi.bmiHeader.biHeight = screen_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        HWND hDesktopWnd = GetDesktopWindow();
        HDC hDesktopDC = GetDC(hDesktopWnd);
        HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
        HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screen_width, screen_height);
        SelectObject(hCaptureDC, hCaptureBitmap);
        BitBlt(hCaptureDC, 0, 0, screen_width, screen_height, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

        RGBQUAD* pPixels = new RGBQUAD[screen_width * screen_height];

        GetDIBits(hCaptureDC, hCaptureBitmap ,0,screen_height,pPixels,&bmi, DIB_RGB_COLORS);

        //getting pixels !
        
        ofstream file("Test.bmp",ios::binary);
        file.write((char*)(&bfh), 14);
        file.write((char*)(&bih), 40);
        int k = 0;
        for(int i=0;i<screen_width;i++)
        {
            for(int j=0;j< screen_width;j++)
            {
                
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;
                if (k < screen_width * screen_height)
                {
                   
                    r = pPixels[k].rgbRed;
                    g = pPixels[k].rgbGreen;
                    b = pPixels[k].rgbBlue;
                    k++;
                }
                
                file << b<<g<<r;
                
            }
        }

        file.close();
        delete[] pPixels;
        ReleaseDC(hDesktopWnd, hDesktopDC);
        DeleteDC(hCaptureDC);
        DeleteObject(hCaptureBitmap);
        return false;
    }
    pah(){
        //Getting width and height of screen !
        screen_width = GetSystemMetrics(SM_CXSCREEN);
        
        screen_height = GetSystemMetrics(SM_CYSCREEN);
        printf("Wysokość głównego ekranu : %i\nSzerokość głównego ekranu : %i", screen_height, screen_width);

        (*point).x = 0;
        (*point).y = 0;
        (*pin_panel_point).x = 0;
        (*pin_panel_point).y = 0;


        //HDC 
        hdc = GetDC(NULL);
        load();
        screen();

    }
   

    

    void main() {
        while(Exit == false)
        {
            auto start = chrono::system_clock::now();
            //If special key is pressed
            if(GetAsyncKeyState(special_key) != 0)
            {
          
                const char* keyboard = read_kb();
                unsigned int length = strlen(keyboard);
                //if keyboard return any output 
                if (length > 0)
                {
                    //control 
                    for (unsigned int i = 0; i < strlen(keyboard); i++)
                    {
                        const char* key = &keyboard[i];
                        switch (*key)
                        {
                            //setting new mouse position
                        case 'P':
                            color = set_color("color");
                            break;
                        case 'L':
                            pin_panel_color = set_color("pin");
                            break;
                        case 'O':
                            injecting = true;
                            break;
                        case 'I':
                            injecting = false;
                            break;
                        case 'R':
                            pin = 0;
                            result[0] = 'N'; result[1] = 'O'; result[2] = 'N'; result[3] = 'E'; result[4] = '\0';
                            break;
                        case 'K':
                            system("cls");
                            cout << "Podaj pin !";
                            cin >> pin;
                            break;
                        case 'N':
                            object += 1;                         
                            if(object > 1)
                            {
                                object = gate;
                            }
                            cout << object;
                            break;
                        
                        }
                        screen();


                    }
                    ////printf("Enter : %s", keyboard);


                }
                
            }
            //if not
            else
            {
                if (injecting)
                {
                    color_check = GetPixel(hdc, (*point).x, (*point).y);
                    pin_panel_color_check = GetPixel(hdc, (*pin_panel_point).x, (*pin_panel_point).y);
                    screen();
                    if (wait > 0)
                    {

                        wait-= elapsed;
                        walk_forward(forward);
                    }
                    else
                    {
                        pin_inject();
                        save();
                        start = chrono::system_clock::now();
                        switch (object)
                        {
                        case gate:
                            forward = true;
                            break;

                        case storage:
                            forward = false;
                            break;
                        }
                    }
                    if(wait_def - wait > 5 && pin_injected) {    
                        int* table = new int[g_s_o_c_size];
                        switch (object)
                        {
                        case gate:
                            //Checking if hack was suceded      
                            get_square_of_colors(table, (*point).x, (*point).y);
                            if ( !color_check_square(table,color) && pin_panel_color_check != pin_panel_color)
                            {
                                forward = false;
                                char_pin(&result[0]);
                                injecting = false;
                                screen();
                                walk_forward(false);
                            }
                            break;
                        case storage:       
                       
                            if(color_check == color)
                            {
                                forward = false;
                                char_pin(&result[0]);
                                injecting = false;
                                screen();
                                walk_forward(false);
                            }
                            break;

                        }
                    }
                    
              
                }
                
            }        
            auto stop = chrono::system_clock::now();
            chrono::duration<double> diff = stop - start;
            elapsed = diff.count();
        }
    }

};
int main()
{
    
    pah* p = new pah();
    p->main();
    //p->print_screen();
    
   
    
    
 
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
