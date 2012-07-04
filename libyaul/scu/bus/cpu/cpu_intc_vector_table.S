/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.macro MACRO_IHR_TRAMPOLINE vector_no
        mov.l r7, @-r15
        bra .L1
        mov #\vector_no, r7
.endm

.type .L3, @function

.L3:
        MACRO_IHR_TRAMPOLINE 0x00

.type .L4, @function

.L4:
        MACRO_IHR_TRAMPOLINE 0x01

.type .L5, @function

.L5:
        MACRO_IHR_TRAMPOLINE 0x02

.type .L6, @function

.L6:
        MACRO_IHR_TRAMPOLINE 0x03

.type .L7, @function

.L7:
        MACRO_IHR_TRAMPOLINE 0x04

.type .L8, @function

.L8:
        MACRO_IHR_TRAMPOLINE 0x05

.type .L9, @function

.L9:
        MACRO_IHR_TRAMPOLINE 0x06

.type .L10, @function

.L10:
        MACRO_IHR_TRAMPOLINE 0x07

.type .L11, @function

.L11:
        MACRO_IHR_TRAMPOLINE 0x08

.type .L12, @function

.L12:
        MACRO_IHR_TRAMPOLINE 0x09

.type .L13, @function

.L13:
        MACRO_IHR_TRAMPOLINE 0x0A

.type .L14, @function

.L14:
        MACRO_IHR_TRAMPOLINE 0x0B

.type .L15, @function

.L15:
        MACRO_IHR_TRAMPOLINE 0x0C

.type .L16, @function

.L16:
        MACRO_IHR_TRAMPOLINE 0x0D

.type .L17, @function

.L17:
        MACRO_IHR_TRAMPOLINE 0x0E

.type .L18, @function

.L18:
        MACRO_IHR_TRAMPOLINE 0x0F

.type .L19, @function

.L19:
        MACRO_IHR_TRAMPOLINE 0x10

.type .L20, @function

.L20:
        MACRO_IHR_TRAMPOLINE 0x11

.type .L21, @function

.L21:
        MACRO_IHR_TRAMPOLINE 0x12

.type .L22, @function

.L22:
        MACRO_IHR_TRAMPOLINE 0x13

.type .L23, @function

.L23:
        MACRO_IHR_TRAMPOLINE 0x14

.type .L24, @function

.L24:
        MACRO_IHR_TRAMPOLINE 0x15

.type .L25, @function

.L25:
        MACRO_IHR_TRAMPOLINE 0x16

.type .L26, @function

.L26:
        MACRO_IHR_TRAMPOLINE 0x17

.type .L27, @function

.L27:
        MACRO_IHR_TRAMPOLINE 0x18

.type .L28, @function

.L28:
        MACRO_IHR_TRAMPOLINE 0x19

.type .L29, @function

.L29:
        MACRO_IHR_TRAMPOLINE 0x1A

.type .L30, @function

.L30:
        MACRO_IHR_TRAMPOLINE 0x1B

.type .L31, @function

.L31:
        MACRO_IHR_TRAMPOLINE 0x1C

.type .L32, @function

.L32:
        MACRO_IHR_TRAMPOLINE 0x1D

.type .L33, @function

.L33:
        MACRO_IHR_TRAMPOLINE 0x1E

.type .L34, @function

.L34:
        MACRO_IHR_TRAMPOLINE 0x1F

.type .L35, @function

.L35:
        MACRO_IHR_TRAMPOLINE 0x20

.type .L36, @function

.L36:
        MACRO_IHR_TRAMPOLINE 0x21

.type .L37, @function

.L37:
        MACRO_IHR_TRAMPOLINE 0x22

.type .L38, @function

.L38:
        MACRO_IHR_TRAMPOLINE 0x23

.type .L39, @function

.L39:
        MACRO_IHR_TRAMPOLINE 0x24

.type .L40, @function

.L40:
        MACRO_IHR_TRAMPOLINE 0x25

.type .L41, @function

.L41:
        MACRO_IHR_TRAMPOLINE 0x26

.type .L42, @function

.L42:
        MACRO_IHR_TRAMPOLINE 0x27

.type .L43, @function

.L43:
        MACRO_IHR_TRAMPOLINE 0x28

.type .L44, @function

.L44:
        MACRO_IHR_TRAMPOLINE 0x29

.type .L45, @function

.L45:
        MACRO_IHR_TRAMPOLINE 0x2A

.type .L46, @function

.L46:
        MACRO_IHR_TRAMPOLINE 0x2B

.type .L47, @function

.L47:
        MACRO_IHR_TRAMPOLINE 0x2C

.type .L48, @function

.L48:
        MACRO_IHR_TRAMPOLINE 0x2D

.type .L49, @function

.L49:
        MACRO_IHR_TRAMPOLINE 0x2E

.type .L50, @function

.L50:
        MACRO_IHR_TRAMPOLINE 0x2F

.type .L51, @function

.L51:
        MACRO_IHR_TRAMPOLINE 0x30

.type .L52, @function

.L52:
        MACRO_IHR_TRAMPOLINE 0x31

.type .L53, @function

.L53:
        MACRO_IHR_TRAMPOLINE 0x32

.type .L54, @function

.L54:
        MACRO_IHR_TRAMPOLINE 0x33

.type .L55, @function

.L55:
        MACRO_IHR_TRAMPOLINE 0x34

.type .L56, @function

.L56:
        MACRO_IHR_TRAMPOLINE 0x35

.type .L57, @function

.L57:
        MACRO_IHR_TRAMPOLINE 0x36

.type .L58, @function

.L58:
        MACRO_IHR_TRAMPOLINE 0x37

.type .L59, @function

.L59:
        MACRO_IHR_TRAMPOLINE 0x38

.type .L60, @function

.L60:
        MACRO_IHR_TRAMPOLINE 0x39

.type .L61, @function

.L61:
        MACRO_IHR_TRAMPOLINE 0x3A

.type .L62, @function

.L62:
        MACRO_IHR_TRAMPOLINE 0x3B

.type .L63, @function

.L63:
        MACRO_IHR_TRAMPOLINE 0x3C

.type .L64, @function

.L64:
        MACRO_IHR_TRAMPOLINE 0x3D

.type .L65, @function

.L65:
        MACRO_IHR_TRAMPOLINE 0x3E

.type .L66, @function

.L66:
        MACRO_IHR_TRAMPOLINE 0x3F

.type .L67, @function

.L67:
        MACRO_IHR_TRAMPOLINE 0x40

.type .L68, @function

.L68:
        MACRO_IHR_TRAMPOLINE 0x41

.type .L69, @function

.L69:
        MACRO_IHR_TRAMPOLINE 0x42

.type .L70, @function

.L70:
        MACRO_IHR_TRAMPOLINE 0x43

.type .L71, @function

.L71:
        MACRO_IHR_TRAMPOLINE 0x44

.type .L72, @function

.L72:
        MACRO_IHR_TRAMPOLINE 0x45

.type .L73, @function

.L73:
        MACRO_IHR_TRAMPOLINE 0x46

.type .L74, @function

.L74:
        MACRO_IHR_TRAMPOLINE 0x47

.type .L75, @function

.L75:
        MACRO_IHR_TRAMPOLINE 0x48

.type .L76, @function

.L76:
        MACRO_IHR_TRAMPOLINE 0x49

.type .L77, @function

.L77:
        MACRO_IHR_TRAMPOLINE 0x4A

.type .L78, @function

.L78:
        MACRO_IHR_TRAMPOLINE 0x4B

.type .L79, @function

.L79:
        MACRO_IHR_TRAMPOLINE 0x4C

.type .L80, @function

.L80:
        MACRO_IHR_TRAMPOLINE 0x4D

.type .L81, @function

.L81:
        MACRO_IHR_TRAMPOLINE 0x4E

.type .L82, @function

.L82:
        MACRO_IHR_TRAMPOLINE 0x4F

.type .L83, @function

.L83:
        MACRO_IHR_TRAMPOLINE 0x50

.type .L84, @function

.L84:
        MACRO_IHR_TRAMPOLINE 0x51

.type .L85, @function

.L85:
        MACRO_IHR_TRAMPOLINE 0x52

.type .L86, @function

.L86:
        MACRO_IHR_TRAMPOLINE 0x53

.type .L87, @function

.L87:
        MACRO_IHR_TRAMPOLINE 0x54

.type .L88, @function

.L88:
        MACRO_IHR_TRAMPOLINE 0x55

.type .L89, @function

.L89:
        MACRO_IHR_TRAMPOLINE 0x56

.type .L90, @function

.L90:
        MACRO_IHR_TRAMPOLINE 0x57

.type .L91, @function

.L91:
        MACRO_IHR_TRAMPOLINE 0x58

.type .L92, @function

.L92:
        MACRO_IHR_TRAMPOLINE 0x59

.type .L93, @function

.L93:
        MACRO_IHR_TRAMPOLINE 0x5A

.type .L94, @function

.L94:
        MACRO_IHR_TRAMPOLINE 0x5B

.type .L95, @function

.L95:
        MACRO_IHR_TRAMPOLINE 0x5C

.type .L96, @function

.L96:
        MACRO_IHR_TRAMPOLINE 0x5D

.type .L97, @function

.L97:
        MACRO_IHR_TRAMPOLINE 0x5E

.type .L98, @function

.L98:
        MACRO_IHR_TRAMPOLINE 0x5F

.type .L99, @function

.L99:
        MACRO_IHR_TRAMPOLINE 0x60

.type .L100, @function

.L100:
        MACRO_IHR_TRAMPOLINE 0x61

.type .L101, @function

.L101:
        MACRO_IHR_TRAMPOLINE 0x62

.type .L102, @function

.L102:
        MACRO_IHR_TRAMPOLINE 0x63

.type .L103, @function

.L103:
        MACRO_IHR_TRAMPOLINE 0x64

.type .L104, @function

.L104:
        MACRO_IHR_TRAMPOLINE 0x65

.type .L105, @function

.L105:
        MACRO_IHR_TRAMPOLINE 0x66

.type .L106, @function

.L106:
        MACRO_IHR_TRAMPOLINE 0x67

.type .L107, @function

.L107:
        MACRO_IHR_TRAMPOLINE 0x68

.type .L108, @function

.L108:
        MACRO_IHR_TRAMPOLINE 0x69

.type .L109, @function

.L109:
        MACRO_IHR_TRAMPOLINE 0x6A

.type .L110, @function

.L110:
        MACRO_IHR_TRAMPOLINE 0x6B

.type .L111, @function

.L111:
        MACRO_IHR_TRAMPOLINE 0x6C

.type .L112, @function

.L112:
        MACRO_IHR_TRAMPOLINE 0x6D

.type .L113, @function

.L113:
        MACRO_IHR_TRAMPOLINE 0x6E

.type .L114, @function

.L114:
        MACRO_IHR_TRAMPOLINE 0x6F

.type .L115, @function

.L115:
        MACRO_IHR_TRAMPOLINE 0x70

.type .L116, @function

.L116:
        MACRO_IHR_TRAMPOLINE 0x71

.type .L117, @function

.L117:
        MACRO_IHR_TRAMPOLINE 0x72

.type .L118, @function

.L118:
        MACRO_IHR_TRAMPOLINE 0x73

.type .L119, @function

.L119:
        MACRO_IHR_TRAMPOLINE 0x74

.type .L120, @function

.L120:
        MACRO_IHR_TRAMPOLINE 0x75

.type .L121, @function

.L121:
        MACRO_IHR_TRAMPOLINE 0x76

.type .L122, @function

.L122:
        MACRO_IHR_TRAMPOLINE 0x77

.type .L123, @function

.L123:
        MACRO_IHR_TRAMPOLINE 0x78

.type .L124, @function

.L124:
        MACRO_IHR_TRAMPOLINE 0x79

.type .L125, @function

.L125:
        MACRO_IHR_TRAMPOLINE 0x7A

.type .L126, @function

.L126:
        MACRO_IHR_TRAMPOLINE 0x7B

.type .L127, @function

.L127:
        MACRO_IHR_TRAMPOLINE 0x7C

.type .L128, @function

.L128:
        MACRO_IHR_TRAMPOLINE 0x7D

.type .L129, @function

.L129:
        MACRO_IHR_TRAMPOLINE 0x7E

.type .L130, @function

.L130:
        MACRO_IHR_TRAMPOLINE 0x7F

.type .L131, @function

.L131:
        MACRO_IHR_TRAMPOLINE 0x80

.type .L132, @function

.L132:
        MACRO_IHR_TRAMPOLINE 0x81

.type .L133, @function

.L133:
        MACRO_IHR_TRAMPOLINE 0x82

.type .L134, @function

.L134:
        MACRO_IHR_TRAMPOLINE 0x83

.type .L135, @function

.L135:
        MACRO_IHR_TRAMPOLINE 0x84

.type .L136, @function

.L136:
        MACRO_IHR_TRAMPOLINE 0x85

.type .L137, @function

.L137:
        MACRO_IHR_TRAMPOLINE 0x86

.type .L138, @function

.L138:
        MACRO_IHR_TRAMPOLINE 0x87

.type .L139, @function

.L139:
        MACRO_IHR_TRAMPOLINE 0x88

.type .L140, @function

.L140:
        MACRO_IHR_TRAMPOLINE 0x89

.type .L141, @function

.L141:
        MACRO_IHR_TRAMPOLINE 0x8A

.type .L142, @function

.L142:
        MACRO_IHR_TRAMPOLINE 0x8B

.type .L143, @function

.L143:
        MACRO_IHR_TRAMPOLINE 0x8C

.type .L144, @function

.L144:
        MACRO_IHR_TRAMPOLINE 0x8D

.type .L145, @function

.L145:
        MACRO_IHR_TRAMPOLINE 0x8E

.type .L146, @function

.L146:
        MACRO_IHR_TRAMPOLINE 0x8F

.type .L147, @function

.L147:
        MACRO_IHR_TRAMPOLINE 0x90

.type .L148, @function

.L148:
        MACRO_IHR_TRAMPOLINE 0x91

.type .L149, @function

.L149:
        MACRO_IHR_TRAMPOLINE 0x92

.type .L150, @function

.L150:
        MACRO_IHR_TRAMPOLINE 0x93

.type .L151, @function

.L151:
        MACRO_IHR_TRAMPOLINE 0x94

.type .L152, @function

.L152:
        MACRO_IHR_TRAMPOLINE 0x95

.type .L153, @function

.L153:
        MACRO_IHR_TRAMPOLINE 0x96

.type .L154, @function

.L154:
        MACRO_IHR_TRAMPOLINE 0x97

.type .L155, @function

.L155:
        MACRO_IHR_TRAMPOLINE 0x98

.type .L156, @function

.L156:
        MACRO_IHR_TRAMPOLINE 0x99

.type .L157, @function

.L157:
        MACRO_IHR_TRAMPOLINE 0x9A

.type .L158, @function

.L158:
        MACRO_IHR_TRAMPOLINE 0x9B

.type .L159, @function

.L159:
        MACRO_IHR_TRAMPOLINE 0x9C

.type .L160, @function

.L160:
        MACRO_IHR_TRAMPOLINE 0x9D

.type .L161, @function

.L161:
        MACRO_IHR_TRAMPOLINE 0x9E

.type .L162, @function

.L162:
        MACRO_IHR_TRAMPOLINE 0x9F

.type .L163, @function

.L163:
        MACRO_IHR_TRAMPOLINE 0xA0

.type .L164, @function

.L164:
        MACRO_IHR_TRAMPOLINE 0xA1

.type .L165, @function

.L165:
        MACRO_IHR_TRAMPOLINE 0xA2

.type .L166, @function

.L166:
        MACRO_IHR_TRAMPOLINE 0xA3

.type .L167, @function

.L167:
        MACRO_IHR_TRAMPOLINE 0xA4

.type .L168, @function

.L168:
        MACRO_IHR_TRAMPOLINE 0xA5

.type .L169, @function

.L169:
        MACRO_IHR_TRAMPOLINE 0xA6

.type .L170, @function

.L170:
        MACRO_IHR_TRAMPOLINE 0xA7

.type .L171, @function

.L171:
        MACRO_IHR_TRAMPOLINE 0xA8

.type .L172, @function

.L172:
        MACRO_IHR_TRAMPOLINE 0xA9

.type .L173, @function

.L173:
        MACRO_IHR_TRAMPOLINE 0xAA

.type .L174, @function

.L174:
        MACRO_IHR_TRAMPOLINE 0xAB

.type .L175, @function

.L175:
        MACRO_IHR_TRAMPOLINE 0xAC

.type .L176, @function

.L176:
        MACRO_IHR_TRAMPOLINE 0xAD

.type .L177, @function

.L177:
        MACRO_IHR_TRAMPOLINE 0xAE

.type .L178, @function

.L178:
        MACRO_IHR_TRAMPOLINE 0xAF

.type .L179, @function

.L179:
        MACRO_IHR_TRAMPOLINE 0xB0

.type .L180, @function

.L180:
        MACRO_IHR_TRAMPOLINE 0xB1

.type .L181, @function

.L181:
        MACRO_IHR_TRAMPOLINE 0xB2

.type .L182, @function

.L182:
        MACRO_IHR_TRAMPOLINE 0xB3

.type .L183, @function

.L183:
        MACRO_IHR_TRAMPOLINE 0xB4

.type .L184, @function

.L184:
        MACRO_IHR_TRAMPOLINE 0xB5

.type .L185, @function

.L185:
        MACRO_IHR_TRAMPOLINE 0xB6

.type .L186, @function

.L186:
        MACRO_IHR_TRAMPOLINE 0xB7

.type .L187, @function

.L187:
        MACRO_IHR_TRAMPOLINE 0xB8

.type .L188, @function

.L188:
        MACRO_IHR_TRAMPOLINE 0xB9

.type .L189, @function

.L189:
        MACRO_IHR_TRAMPOLINE 0xBA

.type .L190, @function

.L190:
        MACRO_IHR_TRAMPOLINE 0xBB

.type .L191, @function

.L191:
        MACRO_IHR_TRAMPOLINE 0xBC

.type .L192, @function

.L192:
        MACRO_IHR_TRAMPOLINE 0xBD

.type .L193, @function

.L193:
        MACRO_IHR_TRAMPOLINE 0xBE

.type .L194, @function

.L194:
        MACRO_IHR_TRAMPOLINE 0xBF

.type .L195, @function

.L195:
        MACRO_IHR_TRAMPOLINE 0xC0

.type .L196, @function

.L196:
        MACRO_IHR_TRAMPOLINE 0xC1

.type .L197, @function

.L197:
        MACRO_IHR_TRAMPOLINE 0xC2

.type .L198, @function

.L198:
        MACRO_IHR_TRAMPOLINE 0xC3

.type .L199, @function

.L199:
        MACRO_IHR_TRAMPOLINE 0xC4

.type .L200, @function

.L200:
        MACRO_IHR_TRAMPOLINE 0xC5

.type .L201, @function

.L201:
        MACRO_IHR_TRAMPOLINE 0xC6

.type .L202, @function

.L202:
        MACRO_IHR_TRAMPOLINE 0xC7

.type .L203, @function

.L203:
        MACRO_IHR_TRAMPOLINE 0xC8

.type .L204, @function

.L204:
        MACRO_IHR_TRAMPOLINE 0xC9

.type .L205, @function

.L205:
        MACRO_IHR_TRAMPOLINE 0xCA

.type .L206, @function

.L206:
        MACRO_IHR_TRAMPOLINE 0xCB

.type .L207, @function

.L207:
        MACRO_IHR_TRAMPOLINE 0xCC

.type .L208, @function

.L208:
        MACRO_IHR_TRAMPOLINE 0xCD

.type .L209, @function

.L209:
        MACRO_IHR_TRAMPOLINE 0xCE

.type .L210, @function

.L210:
        MACRO_IHR_TRAMPOLINE 0xCF

.type .L211, @function

.L211:
        MACRO_IHR_TRAMPOLINE 0xD0

.type .L212, @function

.L212:
        MACRO_IHR_TRAMPOLINE 0xD1

.type .L213, @function

.L213:
        MACRO_IHR_TRAMPOLINE 0xD2

.type .L214, @function

.L214:
        MACRO_IHR_TRAMPOLINE 0xD3

.type .L215, @function

.L215:
        MACRO_IHR_TRAMPOLINE 0xD4

.type .L216, @function

.L216:
        MACRO_IHR_TRAMPOLINE 0xD5

.type .L217, @function

.L217:
        MACRO_IHR_TRAMPOLINE 0xD6

.type .L218, @function

.L218:
        MACRO_IHR_TRAMPOLINE 0xD7

.type .L219, @function

.L219:
        MACRO_IHR_TRAMPOLINE 0xD8

.type .L220, @function

.L220:
        MACRO_IHR_TRAMPOLINE 0xD9

.type .L221, @function

.L221:
        MACRO_IHR_TRAMPOLINE 0xDA

.type .L222, @function

.L222:
        MACRO_IHR_TRAMPOLINE 0xDB

.type .L223, @function

.L223:
        MACRO_IHR_TRAMPOLINE 0xDC

.type .L224, @function

.L224:
        MACRO_IHR_TRAMPOLINE 0xDD

.type .L225, @function

.L225:
        MACRO_IHR_TRAMPOLINE 0xDE

.type .L226, @function

.L226:
        MACRO_IHR_TRAMPOLINE 0xDF

.type .L227, @function

.L227:
        MACRO_IHR_TRAMPOLINE 0xE0

.type .L228, @function

.L228:
        MACRO_IHR_TRAMPOLINE 0xE1

.type .L229, @function

.L229:
        MACRO_IHR_TRAMPOLINE 0xE2

.type .L230, @function

.L230:
        MACRO_IHR_TRAMPOLINE 0xE3

.type .L231, @function

.L231:
        MACRO_IHR_TRAMPOLINE 0xE4

.type .L232, @function

.L232:
        MACRO_IHR_TRAMPOLINE 0xE5

.type .L233, @function

.L233:
        MACRO_IHR_TRAMPOLINE 0xE6

.type .L234, @function

.L234:
        MACRO_IHR_TRAMPOLINE 0xE7

.type .L235, @function

.L235:
        MACRO_IHR_TRAMPOLINE 0xE8

.type .L236, @function

.L236:
        MACRO_IHR_TRAMPOLINE 0xE9

.type .L237, @function

.L237:
        MACRO_IHR_TRAMPOLINE 0xEA

.type .L238, @function

.L238:
        MACRO_IHR_TRAMPOLINE 0xEB

.type .L239, @function

.L239:
        MACRO_IHR_TRAMPOLINE 0xEC

.type .L240, @function

.L240:
        MACRO_IHR_TRAMPOLINE 0xED

.type .L241, @function

.L241:
        MACRO_IHR_TRAMPOLINE 0xEE

.type .L242, @function

.L242:
        MACRO_IHR_TRAMPOLINE 0xEF

.type .L243, @function

.L243:
        MACRO_IHR_TRAMPOLINE 0xF0

.type .L244, @function

.L244:
        MACRO_IHR_TRAMPOLINE 0xF1

.type .L245, @function

.L245:
        MACRO_IHR_TRAMPOLINE 0xF2

.type .L246, @function

.L246:
        MACRO_IHR_TRAMPOLINE 0xF3

.type .L247, @function

.L247:
        MACRO_IHR_TRAMPOLINE 0xF4

.type .L248, @function

.L248:
        MACRO_IHR_TRAMPOLINE 0xF5

.type .L249, @function

.L249:
        MACRO_IHR_TRAMPOLINE 0xF6

.type .L250, @function

.L250:
        MACRO_IHR_TRAMPOLINE 0xF7

.type .L251, @function

.L251:
        MACRO_IHR_TRAMPOLINE 0xF8

.type .L252, @function

.L252:
        MACRO_IHR_TRAMPOLINE 0xF9

.type .L253, @function

.L253:
        MACRO_IHR_TRAMPOLINE 0xFA

.type .L254, @function

.L254:
        MACRO_IHR_TRAMPOLINE 0xFB

.type .L255, @function

.L255:
        MACRO_IHR_TRAMPOLINE 0xFC

.type .L256, @function

.L256:
        MACRO_IHR_TRAMPOLINE 0xFD

.type .L257, @function

.L257:
        MACRO_IHR_TRAMPOLINE 0xFE

.type .L258, @function

.L258:
        MACRO_IHR_TRAMPOLINE 0xFF

.L1:
        /* Trampoline */
        mov.l r6, @-r15
        mov.l r5, @-r15
        mov.l r4, @-r15
        mov.l r3, @-r15
        mov.l r2, @-r15
        mov.l r1, @-r15
        mov.l r0, @-r15
        sts.l pr, @-r15

        /* Disable interrupts */
        mov #0xF0, r1
        extu.b r1, r1
        stc sr, r2
        or r2, r1
        ldc r1, sr

        /* Jump to IHR */
        shll2 r7
        mov.l .LC1, r0
        mov.l @(r0, r7), r7
        /* Expecting the callee follows the calling convention */
        jsr @r7
        nop

        /* Enable interrupts */
        mov #0xFF, r2
        mov #0x0F, r1
        shll8 r2
        stc sr, r3
        or r2, r1
        and r1, r3
        ldc r3, sr

        lds.l @r15+, pr
        mov.l @r15+, r0
        mov.l @r15+, r1
        mov.l @r15+, r2
        mov.l @r15+, r3
        mov.l @r15+, r4
        mov.l @r15+, r5
        mov.l @r15+, r6
        mov.l @r15+, r7
        rte
        nop

.align 2

.LC1:
.long _cpu_intc_vector_table

.data
.align 2

.global _cpu_intc_vector_table
.type _cpu_intc_vector_table, @object
.size _cpu_intc_vector_table, 1024

_cpu_intc_vector_table:
.long .L3
.long .L4
.long .L5
.long .L6
.long .L7
.long .L8
.long .L9
.long .L10
.long .L11
.long .L12
.long .L13
.long .L14
.long .L15
.long .L16
.long .L17
.long .L18
.long .L19
.long .L20
.long .L21
.long .L22
.long .L23
.long .L24
.long .L25
.long .L26
.long .L27
.long .L28
.long .L29
.long .L30
.long .L31
.long .L32
.long .L33
.long .L34
.long .L35
.long .L36
.long .L37
.long .L38
.long .L39
.long .L40
.long .L41
.long .L42
.long .L43
.long .L44
.long .L45
.long .L46
.long .L47
.long .L48
.long .L49
.long .L50
.long .L51
.long .L52
.long .L53
.long .L54
.long .L55
.long .L56
.long .L57
.long .L58
.long .L59
.long .L60
.long .L61
.long .L62
.long .L63
.long .L64
.long .L65
.long .L66
.long .L67
.long .L68
.long .L69
.long .L70
.long .L71
.long .L72
.long .L73
.long .L74
.long .L75
.long .L76
.long .L77
.long .L78
.long .L79
.long .L80
.long .L81
.long .L82
.long .L83
.long .L84
.long .L85
.long .L86
.long .L87
.long .L88
.long .L89
.long .L90
.long .L91
.long .L92
.long .L93
.long .L94
.long .L95
.long .L96
.long .L97
.long .L98
.long .L99
.long .L100
.long .L101
.long .L102
.long .L103
.long .L104
.long .L105
.long .L106
.long .L107
.long .L108
.long .L109
.long .L110
.long .L111
.long .L112
.long .L113
.long .L114
.long .L115
.long .L116
.long .L117
.long .L118
.long .L119
.long .L120
.long .L121
.long .L122
.long .L123
.long .L124
.long .L125
.long .L126
.long .L127
.long .L128
.long .L129
.long .L130
.long .L131
.long .L132
.long .L133
.long .L134
.long .L135
.long .L136
.long .L137
.long .L138
.long .L139
.long .L140
.long .L141
.long .L142
.long .L143
.long .L144
.long .L145
.long .L146
.long .L147
.long .L148
.long .L149
.long .L150
.long .L151
.long .L152
.long .L153
.long .L154
.long .L155
.long .L156
.long .L157
.long .L158
.long .L159
.long .L160
.long .L161
.long .L162
.long .L163
.long .L164
.long .L165
.long .L166
.long .L167
.long .L168
.long .L169
.long .L170
.long .L171
.long .L172
.long .L173
.long .L174
.long .L175
.long .L176
.long .L177
.long .L178
.long .L179
.long .L180
.long .L181
.long .L182
.long .L183
.long .L184
.long .L185
.long .L186
.long .L187
.long .L188
.long .L189
.long .L190
.long .L191
.long .L192
.long .L193
.long .L194
.long .L195
.long .L196
.long .L197
.long .L198
.long .L199
.long .L200
.long .L201
.long .L202
.long .L203
.long .L204
.long .L205
.long .L206
.long .L207
.long .L208
.long .L209
.long .L210
.long .L211
.long .L212
.long .L213
.long .L214
.long .L215
.long .L216
.long .L217
.long .L218
.long .L219
.long .L220
.long .L221
.long .L222
.long .L223
.long .L224
.long .L225
.long .L226
.long .L227
.long .L228
.long .L229
.long .L230
.long .L231
.long .L232
.long .L233
.long .L234
.long .L235
.long .L236
.long .L237
.long .L238
.long .L239
.long .L240
.long .L241
.long .L242
.long .L243
.long .L244
.long .L245
.long .L246
.long .L247
.long .L248
.long .L249
.long .L250
.long .L251
.long .L252
.long .L253
.long .L254
.long .L255
.long .L256
.long .L257
.long .L258
