// CTypeDB.cpp
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
    Clear();    // 기존 데이터 초기화

    CStdioFile file;
    CFileException ex;
    if (!file.Open(filename, CFile::modeRead | CFile::typeText, &ex))
    {
        return FALSE;
    }

    CString line;

    // 1줄(헤더)은 버림
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

        // char
        token = line.Tokenize(_T(","), cur);
        info.m_char = token;

        // type
        token = line.Tokenize(_T(","), cur);
        info.m_type = _ttoi(token);

        // sheet
        token = line.Tokenize(_T(","), cur);
        info.m_sheet = _ttoi(token);

        // sx
        token = line.Tokenize(_T(","), cur);
        info.m_sx = _ttoi(token);

        // sy
        token = line.Tokenize(_T(","), cur);
        info.m_sy = _ttoi(token);

        // line
        token = line.Tokenize(_T(","), cur);
        info.m_line = _ttoi(token);

        // order
        token = line.Tokenize(_T(","), cur);
        info.m_order = _ttoi(token);

        // width
        token = line.Tokenize(_T(","), cur);
        info.m_width = _ttoi(token);

        // height
        token = line.Tokenize(_T(","), cur);
        info.m_height = _ttoi(token);

        m_chars.Add(info);

        // 전체 장 수 / 글자 수 갱신
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
            indices.Add(i);   // m_chars에서의 인덱스를 저장
    }
}
