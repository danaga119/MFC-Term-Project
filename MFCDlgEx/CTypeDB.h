#pragma once

#include <afx.h>
#include <afxtempl.h>   


struct SCharInfo {
    
    SCharInfo() : m_sx(0), m_sy(0), m_width(0), m_height(0),
        m_sheet(0), m_line(0), m_order(0), m_type(0) {
    }

    int m_sx, m_sy, m_width, m_height;
    int m_sheet, m_line, m_order, m_type;
    CString m_char; 
};


class CTypeDB
{
public:
    CTypeDB();
    ~CTypeDB();

    void Clear();

  
    BOOL ReadCSVFile(const CString& filename);

 
    void GetCharsOfSheet(int sheet, CArray<int, int>& indices);

public:
    CArray<SCharInfo, SCharInfo&> m_chars;  
    int m_nSheet;                           
    int m_nChar;                           
};
