#include "pch.h"
#include "CTypeDB.h"

CTypeDB::CTypeDB()
{
    Clear();
}

CTypeDB::~CTypeDB()
{
}

void CTypeDB::Clear()
{
    m_chars.RemoveAll();
    m_nSheet = 0;
    m_nChar = 0;
}

BOOL CTypeDB::ReadCSVFile(const CString& filename)
{
    Clear();   

    CStdioFile file;
    CFileException ex;
    if (!file.Open(filename, CFile::modeRead | CFile::typeText, &ex))
    {
        return FALSE;
    }

    CString line;

    if (!file.ReadString(line))
    {
        file.Close();
        return FALSE;
    }

    while (file.ReadString(line))
    {
        line.Trim();
        if (line.IsEmpty())
            continue;

        int cur = 0;
        CString token;

        SCharInfo info;

 
        token = line.Tokenize(_T(","), cur);
        info.m_char = token;

 
        token = line.Tokenize(_T(","), cur);
        info.m_type = _ttoi(token);

       
        token = line.Tokenize(_T(","), cur);
        info.m_sheet = _ttoi(token);

    
        token = line.Tokenize(_T(","), cur);
        info.m_sx = _ttoi(token);

        token = line.Tokenize(_T(","), cur);
        info.m_sy = _ttoi(token);

  
        token = line.Tokenize(_T(","), cur);
        info.m_line = _ttoi(token);

   
        token = line.Tokenize(_T(","), cur);
        info.m_order = _ttoi(token);

   
        token = line.Tokenize(_T(","), cur);
        info.m_width = _ttoi(token);

     
        token = line.Tokenize(_T(","), cur);
        info.m_height = _ttoi(token);

        m_chars.Add(info);

  
        if (info.m_sheet > m_nSheet)
            m_nSheet = info.m_sheet;
        ++m_nChar;
    }

    file.Close();
    return TRUE;
}

void CTypeDB::GetCharsOfSheet(int sheet, CArray<int, int>& indices)
{
    indices.RemoveAll();

    for (int i = 0; i < m_chars.GetSize(); ++i)
    {
        if (m_chars[i].m_sheet == sheet)
            indices.Add(i);   
    }
}
