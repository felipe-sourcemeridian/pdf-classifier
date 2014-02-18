#include "porter.h"
int cons(int i,stemming *_stemming)
{
//    printf("entry to cons\n");
    switch (_stemming->buffer[i])
    {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return FALSE;
    case 'y':
        return (i==_stemming->k0) ? TRUE : !cons(i-1,_stemming);
    default:
        return TRUE;
    }
}
int m(stemming *_stemming)
{
//    printf("entry to m\n");
    int n = 0;
    int i = _stemming->k0;
    while(TRUE)
    {
        if (i > _stemming->j)
        {
            return n;
        }
        if (!cons(i,_stemming))
        {
            break;
        }
        i++;
    }
    i++;
    while(TRUE)
    {
        while(TRUE)

        {
            if (i > _stemming->j)
            {
                return n;
            }
            if (cons(i,_stemming))
            {
                break;
            }
            i++;
        }
        i++;
        n++;
        while(TRUE)
        {
            if (i > _stemming->j)
            {
                return n;
            }

            if (! cons(i,_stemming))
            {
                break;
            }
            i++;
        }
        i++;
    }
}
int vowelinstem(stemming *_stemming)
{
//    printf("entry vowelinstem\n");
    int i;
    for (i = _stemming->k0; i <= _stemming->j; i++)
    {
        if (! cons(i,_stemming))
        {
            return TRUE;
        }
    }
    return FALSE;
}
int doublec(int j,stemming *_stemming)
{
//    printf("entry doublec \n");
    if (j<_stemming->k0+1)
    {
        return FALSE;
    }
    if (_stemming->buffer[j] != _stemming->buffer[j-1])
    {
        return FALSE;
    }
    return cons(j,_stemming);
}
int cvc(int i,stemming *_stemming)
{
//    printf("entry cvc\n");
    if (i < _stemming->k0+2 || !cons(i,_stemming) || cons(i-1,_stemming) || !cons(i-2,_stemming))
    {
        return FALSE;
    }
    {
        int ch = _stemming->buffer[i];
        if (ch == 'w' || ch == 'x' || ch == 'y')
        {
            return FALSE;
        }
    }
    return TRUE;
}
int ends(char * s,stemming *_stemming)
{
//    printf("entry to ends\n");
    int length = s[0];
    if (s[length] != _stemming->buffer[_stemming->k])
    {
        return FALSE;
    }
    if (length > (_stemming->k-(_stemming->k0+1)))
    {
        return FALSE;
    }
    if (memcmp(_stemming->buffer+_stemming->k-length+1,s+1,length) != 0)
    {
        return FALSE;
    }
    _stemming->j = _stemming->k-length;
    return TRUE;
}
void setto(char * s,stemming *_stemming)
{
//    printf("entry to set to");
    int length = s[0];
    memmove(_stemming->buffer+_stemming->j+1,s+1,length);
    _stemming->k = _stemming->j+length;
}
void r(char * s,stemming *_stemming)
{
//    printf("entry to r\n");
    if (m(_stemming) > 0)
    {
        setto(s,_stemming);
    }
}

/* step1ab() gets rid of plurals and -ed or -ing. e.g.

       caresses  ->  caress
       ponies    ->  poni
       ties      ->  ti
       caress    ->  caress
       cats      ->  cat

       feed      ->  feed
       agreed    ->  agree
       disabled  ->  disable

       matting   ->  mat
       mating    ->  mate
       meeting   ->  meet
       milling   ->  mill
       messing   ->  mess

       meetings  ->  meet

*/

void step1ab(stemming *_stemming)
{
//    printf("init step1 ab\n");
    if (_stemming->buffer[_stemming->k] == 's')
    {
//        printf("contain s \n");
        if (ends("\04" "sses",_stemming))
        {
            _stemming->k -= 2;
        }
        else if (ends("\03" "ies",_stemming))
        {
            setto("\01" "i",_stemming);
        }
        else if (_stemming->buffer[_stemming->k-1] != 's')
        {
            _stemming->k--;
        }
    }
    if (ends("\03" "eed",_stemming))
    {
        if (m(_stemming) > 0)
        {
            _stemming->k--;
        }
    }

    else if ((ends("\02" "ed",_stemming) || ends("\03" "ing",_stemming)) && vowelinstem(_stemming))
    {
        _stemming->k = _stemming->j;
        if (ends("\02" "at",_stemming))
        {
            setto("\03" "ate",_stemming);
        }
        else if (ends("\02" "bl",_stemming))
        {
            setto("\03" "ble",_stemming);
        }
        else if (ends("\02" "iz",_stemming))
        {
            setto("\03" "ize",_stemming);
        }
        else if (doublec(_stemming->k,_stemming))
        {
            _stemming->k--;
            {
                int ch =_stemming->buffer[_stemming->k];
                if (ch == 'l' || ch == 's' || ch == 'z')
                {
                    _stemming->k++;
                }
            }
        }
        else if (m(_stemming) == 1 && cvc(_stemming->k,_stemming))
        {
            setto("\01" "e",_stemming);
        }
    }
}
void step1c(stemming *_stemming)
{
    if (ends("\01" "y",_stemming) && vowelinstem(_stemming))
    {
        _stemming->buffer[_stemming->k] = 'i';
    }
}
/* step2() maps double suffices to single ones. so -ization ( = -ize plus
   -ation) maps to -ize etc. note that the string before the suffix must give
   m() > 0. */

void step2(stemming *_stemming)
{
    switch (_stemming->buffer[_stemming->k-1])

    {

    case 'a':
        if (ends("\07" "ational",_stemming))
        {
            r("\03" "ate",_stemming);
            break;
        }

        if (ends("\06" "tional",_stemming))
        {
            r("\04" "tion",_stemming);
            break;
        }

        break;

    case 'c':
        if (ends("\04" "enci",_stemming))
        {
            r("\04" "ence",_stemming);
            break;
        }
        if (ends("\04" "anci",_stemming))
        {
            r("\04" "ance",_stemming);
            break;
        }
        break;
    case 'e':
        if (ends("\04" "izer",_stemming))
        {
            r("\03" "ize",_stemming);
            break;
        }
        break;

    case 'l':
        if (ends("\03" "bli",_stemming))
        {
            r("\03" "ble",_stemming);
            break;
        } /*-DEPARTURE-*/

        /* To match the published algorithm, replace this line with
           case 'l': if (ends("\04" "abli")) { r("\04" "able"); break; } */
        if (ends("\04" "alli",_stemming))
        {
            r("\02" "al",_stemming);
            break;
        }
        if (ends("\05" "entli",_stemming))
        {
            r("\03" "ent",_stemming);
            break;
        }
        if (ends("\03" "eli",_stemming))
        {
            r("\01" "e",_stemming);
            break;
        }
        if (ends("\05" "ousli",_stemming))
        {
            r("\03" "ous",_stemming);
            break;
        }
        break;

    case 'o':
        if (ends("\07" "ization",_stemming))
        {
            r("\03" "ize",_stemming);
            break;
        }
        if (ends("\05" "ation",_stemming))
        {
            r("\03" "ate",_stemming);
            break;
        }
        if (ends("\04" "ator",_stemming))
        {
            r("\03" "ate",_stemming);
            break;
        }
        break;

    case 's':
        if (ends("\05" "alism",_stemming))
        {
            r("\02" "al",_stemming);
            break;
        }
        if (ends("\07" "iveness",_stemming))
        {
            r("\03" "ive",_stemming);
            break;
        }
        if (ends("\07" "fulness",_stemming))
        {
            r("\03" "ful",_stemming);
            break;
        }
        if (ends("\07" "ousness",_stemming))
        {
            r("\03" "ous",_stemming);
            break;
        }
        break;

    case 't':
        if (ends("\05" "aliti",_stemming))
        {
            r("\02" "al",_stemming);
            break;
        }
        if (ends("\05" "iviti",_stemming))
        {
            r("\03" "ive",_stemming);
            break;
        }
        if (ends("\06" "biliti",_stemming))
        {
            r("\03" "ble",_stemming);
            break;
        }
        break;

    case 'g':
        if (ends("\04" "logi",_stemming))
        {
            r("\03" "log",_stemming);
            break;
        }


    }
}

/* step3() deals with -ic-, -full, -ness etc. similar strategy to step2. */
void step3(stemming *_stemming)
{

    switch (_stemming->buffer[_stemming->k])

    {

    case 'e':
        if (ends("\05" "icate",_stemming))
        {

            r("\02" "ic",_stemming);
            break;
        }
        if (ends("\05" "ative",_stemming))
        {
            r("\00" "",_stemming);
            break;
        }

        if (ends("\05" "alize",_stemming))
        {

            r("\02" "al",_stemming);
            break;
        }
        break;

    case 'i':
        if (ends("\05" "iciti",_stemming))
        {
            r("\02" "ic",_stemming);
            break;
        }
        break;
    case 'l':
        if (ends("\04" "ical",_stemming))
        {
            r("\02" "ic",_stemming);
            break;
        }

        if (ends("\03" "ful",_stemming))
        {
            r("\00" "",_stemming);
            break;
        }
        break;

    case 's':
        if (ends("\04" "ness",_stemming))
        {
            r("\00" "",_stemming);
            break;
        }
        break;
    }
}
/* step4() takes off -ant, -ence etc., in context <c>vcvc<v>. */
void step4(stemming *_stemming)
{
    switch (_stemming->buffer[_stemming->k-1])

    {

    case 'a':
        if (ends("\02" "al",_stemming))
        {
            break;
        }
        return;


    case 'c':
        if (ends("\04" "ance",_stemming))
        {
            break;
        }
        if (ends("\04" "ence",_stemming))
        {
            break;
        }
        return;
    case 'e':
        if (ends("\02" "er",_stemming))
        {
            break;
        }
        return;
    case 'i':
        if (ends("\02" "ic",_stemming))
        {
            break;
        }
        return;
    case 'l':
        if (ends("\04" "able",_stemming))
        {
            break;
        }
        if (ends("\04" "ible",_stemming))
        {

            break;
        }
        return;

    case 'n':
        if (ends("\03" "ant",_stemming))
        {
            break;
        }

        if (ends("\05" "ement",_stemming)) break;

        if (ends("\04" "ment",_stemming)) break;

        if (ends("\03" "ent",_stemming)) break;
        return;

    case 'o':
        if (ends("\03" "ion",_stemming) && (_stemming->buffer[_stemming->j] == 's' ||_stemming->buffer[_stemming->j] == 't'))
        {
            break;
        }
        if (ends("\02" "ou",_stemming))
        {
            break;
        }
        return;
        /* takes care of -ous */

    case 's':
        if (ends("\03" "ism",_stemming))
        {
            break;
        }
        return;

    case 't':
        if (ends("\03" "ate",_stemming))
        {
            break;
        }
        if (ends("\03" "iti",_stemming))
        {
            break;
        }
        return;

    case 'u':
        if (ends("\03" "ous",_stemming))
        {
            break;
        }
        return;

    case 'v':
        if (ends("\03" "ive",_stemming))
        {
            break;
        }
        return;

    case 'z':
        if (ends("\03" "ize",_stemming))
        {
            break;
        }
        return;
    default:
        return;
    }
    if (m(_stemming) > 1)
    {
        _stemming->k = _stemming->j;
    }
}
/* step5() removes a final -e if m() > 1, and changes -ll to -l if
   m() > 1. */
void step5(stemming *_stemming)
{
    _stemming->j = _stemming->k;
    if(_stemming->buffer[_stemming->k] == 'e')
    {
        int a = m(_stemming);
        if (a > 1 || (a == 1 && !cvc(_stemming->k-1,_stemming)))
        {
            _stemming->k--;
        }
    }
    if (_stemming->buffer[_stemming->k] == 'l' && doublec(_stemming->k,_stemming) && m(_stemming) > 1)
    {
        _stemming->k--;
    }
}
/* In stem(p,i,j), p is a char pointer, and the string to be stemmed is from
   p[i] to p[j] inclusive. Typically i is zero and j is the offset to the last
   character of a string, (p[j+1] == '\0'). The stemmer adjusts the
   characters p[i] ... p[j] and returns the new end-point of the string, k.
   Stemming never increases word length, so i <= k <= j. To turn the stemmer
   into a module, declare 'stem' as extern, and delete the remainder of this
   file.
*/

int stem(stemming *_stemming)
{
    if (_stemming->k <= _stemming->k0+1)
    {
        return _stemming->k;
    }
    step1ab(_stemming);
    step1c(_stemming);
    step2(_stemming);
    step3(_stemming);
    step4(_stemming);
    step5(_stemming);
    return _stemming->k;
}
stemming *build_stemming(memory_page_buffer **_memory_page_buffer)
{
	memory_page_list *_memory_page_list=NULL;
	stemming *_stemming=(stemming *)malloc(sizeof(stemming));
	if(_stemming==NULL)
	{
		return NULL;
	}
	_memory_page_list=create_page();
	if(_memory_page_list==NULL)
	{
		free(_stemming);
		return NULL;
	}
	_stemming->page=_memory_page_list;
	*_memory_page_buffer=add_page_to_buffer(*_memory_page_buffer,_memory_page_list);
	_stemming->buffer=(char *)_memory_page_list->addr;
	_stemming->size=_memory_page_list->memory_size;
	return _stemming;
}
void delete_stemming(stemming *_stemming)
{
	free(_stemming);
}
