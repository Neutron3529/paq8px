#include "NestModel.hpp"

NestModel::NestModel(const uint64_t size) : cm(size, nCM) {}

void NestModel::mix(Mixer &m) {
  if( shared->bitPosition == 0 ) {
    int c = shared->c1;
    int matched = 1;
    int vv;
    w *= ((vc & 7U) > 0 && (vc & 7U) < 3);
    if( c & 0x80U ){
      w = w * 11 * 32 + c;
    }
    const int lc = (c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c);
    if( lc == 'a' || lc == 'e' || lc == 'i' || lc == 'o' || lc == 'u' ) {
      vv = 1;
      w = w * 997 * 8 + (lc / 4 - 22);
    } else if( lc >= 'a' && lc <= 'z' ) {
      vv = 2;
      w = w * 271 * 32 + lc - 97;
    } else if( lc == ' ' || lc == '.' || lc == ',' || lc == '\n' ) {
      vv = 3;
    } else if( lc >= '0' && lc <= '9' ) {
      vv = 4;
    } else if( lc == 'y' ) {
      vv = 5;
    } else if( lc == '\'' ) {
      vv = 6;
    } else {
      vv = (c & 32U) != 0u ? 7 : 0;
    }
    vc = (vc << 3U) | vv;
    if( vv != lvc ) {
      wc = (wc << 3U) | vv;
      lvc = vv;
    }
    switch( c ) {
      case ' ':
        qc = 0;
        break;
      case '(':
        ic += 31;
        break;
      case ')':
        ic -= 31;
        break;
      case '[':
        ic += 11;
        break;
      case ']':
        ic -= 11;
        break;
      case '<':
        ic += 23;
        qc += 34;
        break;
      case '>':
        ic -= 23;
        qc /= 5;
        break;
      case ':':
        pc = 20;
        break;
      case '{':
        ic += 17;
        break;
      case '}':
        ic -= 17;
        break;
      case '|':
        pc += 223;
        break;
      case '"':
        pc += 0x40;
        break;
      case '\'':
        pc += 0x42;
        if( c != static_cast<uint8_t>(shared->c4 >> 8U)) {
          sense2 ^= 1;
        } else {
          ac += (2 * sense2 - 1);
        }
        break;
      case '\n':
        pc = qc = 0;
        break;
      case '.':
      case '!':
      case '?':
        pc = 0;
        break;
      case '#':
        pc += 0x08;
        break;
      case '%':
        pc += 0x76;
        break;
      case '$':
        pc += 0x45;
        break;
      case '*':
        pc += 0x35;
        break;
      case '-':
        pc += 0x3;
        break;
      case '@':
        pc += 0x72;
        break;
      case '&':
        qc += 0x12;
        break;
      case ';':
        qc /= 3;
        break;
      case '\\':
        pc += 0x29;
        break;
      case '/':
        pc += 0x11;
        if( shared->c1 == '<' ) {
          qc += 74;
        }
        break;
      case '=':
        pc += 87;
        if( c != static_cast<uint8_t>(shared->c4 >> 8U)) {
          sense1 ^= 1U;
        } else {
          ec += (2 * sense1 - 1);
        }
        break;
      default:
        matched = 0;
    }
    if( shared->c4 == 0x266C743B ) {
      uc = min(7, uc + 1); //&lt;
    } else if( shared->c4 == 0x2667743B ) {
      uc -= static_cast<int>(uc > 0); //&gt;
    }
    if( matched != 0 ) {
      bc = 0;
    } else {
      bc += 1;
    }
    if( bc > 300 ) {
      bc = ic = pc = qc = uc = 0;
    }
    uint64_t i = 0;
    cm.set(hash(++i, (vv > 0 && vv < 3) ? 0 : (lc | 0x100U), ic & 0x3FFU, ec & 0x7U, ac & 0x7U, uc));
    cm.set(hash(++i, ic, w, ilog2(bc + 1)));
    cm.set(hash(++i, (3 * vc + 77 * pc + 373 * ic + qc) & 0xffffu));
    cm.set(hash(++i, (31 * vc + 27 * pc + 281 * qc) & 0xffffu));
    cm.set(hash(++i, (13 * vc + 271 * ic + qc + bc) & 0xffffu));
    cm.set(hash(++i, (17 * pc + 7 * ic) & 0xffffu));
    cm.set(hash(++i, (13 * vc + ic) & 0xffffu));
    cm.set(hash(++i, (vc / 3 + pc) & 0xffffu));
    cm.set(hash(++i, (7 * wc + qc) & 0xffffu));
    cm.set(hash(++i, vc & 0xffffu, shared->c4 & 0xffu));
    cm.set(hash(++i, (3 * pc) & 0xffffu, shared->c4 & 0xffu));
    cm.set(hash(++i, ic & 0xffffu, shared->c4 & 0xffu));
  }
  cm.mix(m);
}
