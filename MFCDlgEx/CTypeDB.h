// CTypeDB.h
#pragma once

#include <afx.h>
#include <afxtempl.h>   // CArray

// CSV 한 줄에 해당하는 글자 정보 구조체

struct SCharInfo {
    // 생성자 추가: 모든 멤버를 초기화합니다.
    SCharInfo() : m_sx(0), m_sy(0), m_width(0), m_height(0),
        m_sheet(0), m_line(0), m_order(0), m_type(0) {
    }

    int m_sx, m_sy, m_width, m_height;
    int m_sheet, m_line, m_order, m_type;
    CString m_char; // CString은 자동으로 빈 문자열로 초기화됩니다.
};

// 글자 DB 클래스
class CTypeDB
{
public:
    CTypeDB();
    ~CTypeDB();

    // 모든 데이터 초기화
    void Clear();

    // CSV 파일 읽어서 m_chars, m_nSheet, m_nChar 채우기
    BOOL ReadCSVFile(const CString& filename);

    // 특정 장에 해당하는 m_chars 인덱스 목록 얻기
    void GetCharsOfSheet(int sheet, CArray<int, int>& indices);

public:
    CArray<SCharInfo, SCharInfo&> m_chars;  // 전체 글자 정보
    int m_nSheet;                           // 전체 장 수
    int m_nChar;                            // 전체 글자 수
};
