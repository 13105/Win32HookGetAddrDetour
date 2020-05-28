

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "errors.h"
using namespace std;

void cerr_lastError(){

	char err_msg[300];
	
	DWORD ErrCode = GetLastError();
	cout << "Error code: " << ErrCode << endl;
	if ( ErrCode > 0){
		
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM,
			0,
			ErrCode,
			LANG_SYSTEM_DEFAULT,
			err_msg,
			sizeof(err_msg),
			NULL
		);
		
		cout << err_msg << endl;
	}
	
	
			
	
	
	
	
}