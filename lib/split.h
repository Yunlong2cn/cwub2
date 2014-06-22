
#include <string.h>
#include <malloc.h>

class StringSplit
{
private:
    wchar_t *str;
    int *offset;
    int count;
public:
    StringSplit(const wchar_t *strSource,const wchar_t wch,int length = -1)
    {
        str = 0;
        offset = 0;
        count = 0;
        
        int len = (length==-1)?wcslen(strSource):length;
        if(len>0)
        {
            str = (wchar_t *) malloc((len + 2) * sizeof(wchar_t));
            memcpy(str,strSource,(len + 2) * sizeof(wchar_t));
            offset = (int*)malloc((len + 1) * sizeof(int));//����len��һ��
            
            offset[0] = 0;
            
            //����ĩβ���ַ�
            if(str[len - 1]!=wch)
            {
                str[len++] = wch;
                str[len++] = 0;
            }
            
            //�����ַ���
            for(int i=0;i<len;i++)
            {
                if(str[i]==wch)
                {
                    str[i] = 0;
                    count++;
                    offset[count] = i + 1;
                }
            }
        }
    }
    //���� 
    ~StringSplit()
    {
        if(str) free(str);
        if(offset) free(offset);
    }
    
    int GetCount()
    {
        return count;
    }
    
    wchar_t *GetIndex(int index)
    {
        return &str[offset[index]];
    }
    
};
