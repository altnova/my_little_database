//! \file stm.c \brief porter stemmer

#include <string.h>
#include "___.h"
#include "stm.h"

/* This is the Porter stemming algorithm, coded up in ANSI C by the
 * author. It may be be regarded as canonical, in that it follows the
 * algorithm presented in:
 *
 * Porter, 1980, An algorithm for suffix stripping, Program, Vol. 14,
 * no. 3, pp 130-137,
 *
 * only differing from it at the points marked --DEPARTURE-- below.
 *
 * See also http://www.tartarus.org/~martin/PorterStemmer
 *
 * The algorithm as described in the paper could be exactly replicated
 * by adjusting the points of DEPARTURE, but this is barely necessary,
 * because (a) the points of DEPARTURE are definitely improvements, and
 * (b) no encoding of the Porter stemmer I have seen is anything like
 * as exact as this version, even with the points of DEPARTURE!
 *
 * You can compile it on Unix with 'gcc -O3 -o stem stem.c' after which
 * 'stem' takes a list of inputs and sends the stemmed equivalent to
 * stdout.
 *
 * The algorithm as encoded here is particularly fast.
 *
 * Release 1: was many years ago
 * Release 2: 11 Apr 2013
 *     fixes a bug noted by Matt Patenaude <matt@mattpatenaude.com>,
 *
 *     case 'o': if (ends("\03" "ion") && (b[j] == 's' || b[j] == 't')) break;
 *         ==>
 *     case 'o': if (ends("\03" "ion") && j >= k0 && (b[j] == 's' || b[j] == 't')) break;
 *
 *     to avoid accessing b[k0-1] when the word in b is "ion".
 * Release 3: 25 Mar 2014
 *     fixes a similar bug noted by Klemens Baum <klemensbaum@gmail.com>,
 *     that if step1ab leaves a one letter result (ied -> i, aing -> a etc),
 *     step2 and step4 access the byte before the first letter. So we skip
 *     steps after step1ab unless k > k0. */

/* The main part of the stemming algorithm starts here. b is a buffer
 * holding a word to be stemmed. The letters are in b[k0], b[k0+1] ...
 * ending at b[k]. In fact k0 = 0 in this demo program. k is readjusted
 * downwards as the stemming progresses. Zero termination is not in fact
 * used in the algorithm.
 *
 * Note that only lower case sequences are stemmed. Forcing to lower case
 * should be done before stem(...) is called. */

/* buffer for word to be stemmed */
ZS b;
ZI k;
ZI k0;
/* j is a general offset into the string */
ZI j;
const ZS VOWELS = "*************************************************************************************************a   e   i     o     u     ";
/**
 * TRUE when `b[i]` is a consonant.
 */

Z inline I isConsonant(I i) {
  C c = b[i];

  //           abcdefghijklmnopqrstuvwxyz

  I r =
    //c=='a'||c=='e'||c=='i'||c=='o'||c=='u'?FALSE:
    VOWELS[c]!=' '   ?FALSE:
    c=='y'           ?(i==k0)?TRUE:!isConsonant(i-1)
                     :TRUE;
  R r;
}

/* Measure the number of consonant sequences between
 * `k0` and `j`.  If C is a consonant sequence and V
 * a vowel sequence, and <..> indicates arbitrary
 * presence:
 *
 *   <C><V>       gives 0
 *   <C>VC<V>     gives 1
 *   <C>VCVC<V>   gives 2
 *   <C>VCVCVC<V> gives 3
 *   ....
 */
Z inline I getMeasure() {
  I pos=0, i=k0;
  W(TRUE)
    goto LOOP;
  i++;
  W(TRUE){
    W(TRUE){
      if(i>j)R pos;
      if(isConsonant(i))break;
      i++;
    }
    i++; pos++;
    W(TRUE){
      LOOP:
      if(i>j)R pos;
      if(!isConsonant(i))break;
      i++;
    }
    i++;
  }
}

/* `TRUE` when `k0, ... j` contains a vowel. */
Z inline I vowelInStem(){
  I i = k0 - 1;
  W(++i<=j)
    if(!isConsonant(i))R TRUE;
  R FALSE;
}

/* `TRUE` when `j` and `(j-1)` are the same consonant. */
Z inline I isDoubleConsonant(I i) {
  R(b[i]!=b[i-1])?FALSE:isConsonant(i);
}

/* `TRUE` when `i - 2, i - 1, i` has the form
 * `consonant - vowel - consonant` and also if the second
 * C is not `"w"`, `"x"`, or `"y"`. this is used when
 * trying to restore an `e` at the end of a short word.
 *
 * Such as:
 *
 * `cav(e)`, `lov(e)`, `hop(e)`, `crim(e)`, but `snow`,
 * `box`, `tray`.
 */
ZI cvc(I i){
  I c = b[i];
  if(i<k0+2
      ||!isConsonant(i)
      || isConsonant(i-1)
      ||!isConsonant(i-2))
    R FALSE;
  if(c=='w'||c =='x'||c=='y')
    R FALSE;
  R TRUE;
}

/* `ends(s)` is `TRUE` when `k0, ...k` ends with `value`. */
Z inline I ends(const S v){
  I l=v[0];
  if(v[l]!=b[k]||l>k-k0+1)R FALSE;
  DO(l,P((b+k-l+1)[i]!=(v+1)[i],FALSE))
  //if(memcmp(b+k-l+1, v+1, l)!=0)R FALSE;
  j=k-l;R TRUE;
}

/* `setTo(value)` sets `(j + 1), ...k` to the characters in
 * `value`, readjusting `k`. */
ZV setTo(const S v){
  I l = v[0];
  memmove(b+j+1, v+1, l);
  k=j+l;
}

/* Set string. */
ZV replace(const S v){
  if(getMeasure())
    setTo(v);
}

/* `step1ab()` gets rid of plurals, `-ed`, `-ing`.
 *
 * Such as:
 *
 *   caresses  ->  caress
 *   ponies    ->  poni
 *   ties      ->  ti
 *   caress    ->  caress
 *   cats      ->  cat
 *
 *   feed      ->  feed
 *   agreed    ->  agree
 *   disabled  ->  disable
 *
 *   matting   ->  mat
 *   mating    ->  mate
 *   meeting   ->  meet
 *   milling   ->  mill
 *   messing   ->  mess
 *
 *   meetings  ->  meet
 */
ZV step1ab() {
  I character;

  if (b[k]=='s') {
    if (ends("\04" "sses")) {
      k -= 2;
    } else if (ends("\03" "ies")) {
      setTo("\01" "i");
    } else if (b[k-1]!='s') {
      k--;
    }
  }
  if (ends("\03" "eed")) {
    if (getMeasure() > 0) {
      k--;
    }
  } else if ((ends("\02" "ed") || ends("\03" "ing")) && vowelInStem()) {
    k = j;

    if (ends("\02" "at")) {
      setTo("\03" "ate");
    } else if (ends("\02" "bl")) {
      setTo("\03" "ble");
    } else if (ends("\02" "iz")) {
      setTo("\03" "ize");
    } else if (isDoubleConsonant(k)) {
      k--;

      character = b[k];

      if (character == 'l' || character == 's' || character == 'z') {
        k++;
      }
    } else if (getMeasure() == 1 && cvc(k)) {
      setTo("\01" "e");
    }
  }
}

/* `step1c()` turns terminal `"y"` to `"i"` when there
 * is another vowel in the stem. */
ZV step1c() {
  if(ends("\01" "y")&&vowelInStem())
    b[k] = 'i';
}

/* `step2()` maps double suffices to single ones.
 * so -ization ( = -ize plus -ation) maps to -ize etc.
 * note that the string before the suffix must give
 * getMeasure() > 0. */
ZV step2(){

  I l = k-k0+1;

  SW(b[k-1]){

    CS('a',
      if(b[k]!='l')
        break;      

      if (ends("\07" "ational")) {
        replace("\03" "ate");
        break;
      }
      if (ends("\06" "tional")) {
        replace("\04" "tion");
        break;
      })

    CS('c',
      if(b[k]!='i')
        break;

      if (ends("\04" "enci")) {
        replace("\04" "ence");
        break;
      }
      if (ends("\04" "anci")) {
        replace("\04" "ance");
        break;
      })

    CS('e',
      if(ends("\04" "izer"))
        replace("\03" "ize"))

    CS('l',
      /* --DEPARTURE--: To match the published algorithm,
       * replace this line with:
       *
       * ```
       * if (ends("\04" "abli")) {
       *     replace("\04" "able");
       *
       *     break;
       * }
       * ```
       */
      if(b[k]!='i') break;

      if(ends("\03" "bli")) {
        replace("\03" "ble");
        break;
      }
      if (ends("\04" "alli")) {
        replace("\02" "al");
        break;
      }
      if (ends("\05" "entli")) {
        replace("\03" "ent");
        break;
      }
      if (ends("\03" "eli")) {
        replace("\01" "e");
        break;
      }
      if (ends("\05" "ousli")) {
        replace("\03" "ous");
        break;
      })

    CS('o',

      if(!rchr(b,'a'))break;

      if (ends("\07" "ization")) {
        replace("\03" "ize");
        break;
      }

      if (ends("\05" "ation")) {
        replace("\03" "ate");
        break;
      }

      if (ends("\04" "ator")) {
        replace("\03" "ate");
        break;
      })


    CS('s',
      if (ends("\05" "alism")) {
        replace("\02" "al");
        break;
      }

      if(b[k-1]!='s'||b[k-2]!='e')break;

      if (ends("\07" "iveness")) {
        replace("\03" "ive");
        break;
      }

      if (ends("\07" "fulness")) {
        replace("\03" "ful");
        break;
      }

      if (ends("\07" "ousness")) {
        replace("\03" "ous");
        break;
      })

    CS('t',
      if (b[k]!='i'||b[k-2]!='i')break;

      if (ends("\05" "aliti")) {
        replace("\02" "al");
        break;
      }
      if (ends("\05" "iviti")) {
        replace("\03" "ive");
        break;
      }
      if (ends("\06" "biliti")) {
        replace("\03" "ble");
        break;
      })
    /* --DEPARTURE--: To match the published algorithm, delete this line. */
    CS('g',
      if (ends("\04" "logi"))
        replace("\03" "log"))
  }
}

/* `step3()` deals with -ic-, -full, -ness etc.
 * similar strategy to step2. */
ZV step3() {
  I l = k-k0+1;
  SW(b[k]){

    CS('e',
      if(!rchr(b,'a')) {break;}

      if(ends("\05" "icate")) {
        replace("\02" "ic");
        break;
      }
      if (ends("\05" "ative")) {
        replace("\00" "");
        break;
      }
      if (ends("\05" "alize")) {
        replace("\02" "al");
        break;
      })

    CS('i',
      if (ends("\05" "iciti")) {
        replace("\02" "ic");
        break;
      })

    CS('l',
      if (ends("\04" "ical")) {
        replace("\02" "ic");
        break;
      }
      if (ends("\03" "ful")) {
        replace("\00" "");
        break;
      })

    CS('s',
      if (ends("\04" "ness"))
        replace("\00" ""))
  }
}

/* `step4()` takes off -ant, -ence etc., in
 * context <c>vcvc<v>. */
ZV step4() {
  SW(b[k-1]){

    CS('a',
      if(ends("\02" "al"))break;
      R;)

    CS('c',
      if(ends("\04" "ance"))break;
      if(ends("\04" "ence"))break;
      R;)

    CS('e',
      if(ends("\02" "er"))break;
      R;)

    CS('i',
      if(ends("\02" "ic"))break;
      R;)

    CS('l',
      if(ends("\04" "able"))break;
      if(ends("\04" "ible"))break;
      R;)

    CS('n',
      if(ends("\03" "ant"))break;
      if(ends("\05" "ement"))break;
      if(ends("\04" "ment"))break;
      if(ends("\03" "ent"))break;
      R;)

    CS('o',
      if(ends("\03" "ion")&&j>=k0&&(b[j]=='s'||b[j]=='t'))break;
      if(ends("\02" "ou"))break; //< takes care of -ous
      R;)

    CS('s',
      if(ends("\03" "ism"))break;
      R;)

    CS('t',
      if(ends("\03" "ate"))break;
      if(ends("\03" "iti"))break;
      R;)

    CS('u',
      if(ends("\03" "ous"))break;
      R;)

    CS('v',
      if(ends("\03" "ive"))break;
      R;)

    CS('z',
      if (ends("\03" "ize"))break;
      R;)

    CD:R;
  }

  if(getMeasure()>1)
    k=j;
}

/* `step5()` removes a final `-e` if `getMeasure()` is
 * greater than `1`, and changes `-ll` to `-l` if
 * `getMeasure()` is greater than `1`. */
ZV step5() {
  I a;
  j = k;

  if(b[k]=='e') {
    a = getMeasure();
    if (a>1||(a==1 && !cvc(k - 1)))
      k--;
  }

  if(b[k]=='l' && isDoubleConsonant(k) && getMeasure()>1)
    k--;
}

/* In `stem(p, i, j)`, `p` is a `char` pointer, and the
 * string to be stemmed is from `p[i]` to
 * `p[j]` (inclusive).
 *
 * Typically, `i` is zero and `j` is the offset to the
 * last character of a string, `(p[j + 1] == '\0')`.
 * The stemmer adjusts the characters `p[i]` ... `p[j]`
 * and returns the new end-point of the string, `k`.
 *
 * Stemming never increases word length, so `i <= k <= j`.
 *
 * To turn the stemmer into a module, declare 'stm' as
 * extern, and delete the remainder of this file. */
I stm(S p, I i, I pos){
  /* Copy the parameters into statics. */
  b = p;
  k = pos;
  k0 = i;

  if(k<=k0+1)
    R k; /* --DEPARTURE-- */

  /* With this line, strings of length 1 or 2 don't
   * go through the stemming process, although no
   * mention is made of this in the published
   * algorithm. Remove the line to match the published
   * algorithm. */
  step1ab();
  if(k>k0){
    step1c();
    step2();
    step3();
    step4();
    step5();
  }

  R k;
}


#ifdef RUN_TESTS_STM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fio.h"
#include "trc.h"

ZI assertionCount = 0, errorCount = 0;

ZV assertStem(const S input, const S output){
  S value, result, fixture;

  value = strdup(input);
  result = strdup(input);
  fixture = strdup(output);

  result[stm(result, 0, scnt(result) - 1) + 1] = 0;

  if (strcmp(fixture, result)) {
    errorCount++;
    O("\033[31m");
    O("  (✖) For `%s`. Expected `%s`, got `%s`", value, fixture, result);
    O("\033[0m");
    O("\n");
  } else
//    O("\033[32m.\033[0m");
  assertionCount++;

  free(value);
  free(fixture);
  free(result);
}

I main(){
  LOG("stm_test");
  FILE*input,*output;

  S lineIn, lineOut;
  sz lengthIn, lengthOut;

  lineIn = NULL;
  lineOut = NULL;
  lengthIn = 0;
  lengthOut = 0;

  xfopen(input,  "fxt/input.txt", "r", 1);
  xfopen(output, "fxt/output.txt", "r", 1);

  W(
    getline(&lineIn, &lengthIn, input)!=-1 &&
    getline(&lineOut, &lengthOut, output)!=-1
  ){
    lineIn[scnt(lineIn) - 1] = 0;
    lineOut[scnt(lineOut) - 1] = 0;
    assertStem(lineIn, lineOut);
  }

  assertStem("nationalization", "nation");
  assertStem("nationalism", "nation");

  if(errorCount) {
    O("\033[31m");
    O("(✖) Failed on %d of %d assertions", errorCount, assertionCount);
    O("\033[0m");
    O("\n");
    exit(EXIT_FAILURE);
  }

  O("\033[32m");
  O("(✓) Passed %d assertions without errors", assertionCount);
  O("\033[0m");
  O("\n");

  fclose(input);
  fclose(output);

  R0;
}

#endif



//:~