// SYNTAX TEST "Packages/Ethanon/Ethanon.sublime-syntax"
#include "src/foo.angelscript"
// <- meta.preprocessor.include punctuation.definition.preprocessor
//       ^^^^^^^^^^^^^^^^^^^^^ string.quoted.double.include
#if TESTING
//  ^^^^^^^ support.constant.preprocessor
#endif

funcdef sef::Font@ FONT_CREATOR(const string &in);
// <- keyword.declaration.funcdef
//      ^^^ variable.namespace
//                 ^^^^^^^^^^^^ entity.name.type.funcdef
//                              ^^^^^ storage.modifier
//                                    ^^^^^^ support.class
//                                           ^^^ storage.modifier.reference

enum REQUEST_STATUS
//   ^^^^^^^^^^^^^^ entity.name.enum
{
    RS_SUCCESS,
//  ^^^^^^^^^^ entity.name.constant.enum
    RS_FAILED = 2
//  ^^^^^^^^^ entity.name.constant.enum
//              ^ constant.numeric
}

namespace sef {
// <- keyword.declaration.namespace
//        ^^^ entity.name.namespace

abstract class UIEffect : RequestReceiver, sef::Event
// <- storage.modifier
//       ^^^^^ keyword.declaration.class
//             ^^^^^^^^ entity.name.class
//                        ^^^^^^^^^^^^^^^ entity.other.inherited-class
//                                              ^^^^^ entity.other.inherited-class
{
    private float m_elapsed = 0.0f;
//  ^^^^^^^ storage.modifier
//          ^^^^^ storage.type
//                            ^^^^ constant.numeric
//                                ^ punctuation.terminator

    UIEffect(const string &in url, const float timeOutMs)
//  ^^^^^^^^ entity.name.function
    {
        m_url = (url != "") ? url : "%NO_URL%";
//                   ^^ keyword.operator.comparison
//                      ^^ string.quoted.double
        PlaySample("a.ogg", 2000.0f);
//      ^^^^^^^^^^ support.function
        FadeOutSample("a.ogg", 500);
//      ^^^^^^^^^^^^^ support.function
        sendRequest(m_url);
//      ^^^^^^^^^^^ variable.function - entity
        Foo foo(1, 2);
//          ^^^ variable.function - entity
    }

    ~UIEffect() {}
//  ^^^^^^^^^ entity.name.function

    sef::Font@ getFont() const override
//             ^^^^^^^ entity.name.function
//                       ^^^^^ storage.modifier
//                             ^^^^^^^^ storage.modifier
    {
        if (m_font !is null && x is y)
//      ^^ keyword.control
//                 ^^^ keyword.operator.word
//                     ^^^^ constant.language.null
//                               ^^ keyword.operator.word
            return cast<sef::Font>(m_font);
//          ^^^^^^ keyword.control
//                 ^^^^ keyword.operator.word.cast
        return null;
    }

    bool isOk { get const { return true; } }
//              ^^^ keyword.declaration.function.accessor
//                                 ^^^^ constant.language.boolean

    void abstractOne();
//       ^^^^^^^^^^^ entity.name.function
}

} // namespace sef

array<ETHEntity@>@ getEntities(const vector2 &in pos, uint mask = 0xFF00)
// <- support.class
//    ^^^^^^^^^ support.class
//             ^ storage.modifier.handle
//                 ^^^^^^^^^^^ entity.name.function
//                                                                ^^^^^^ constant.numeric.integer.hexadecimal
{
    ETHEntityArray ents;
    GetEntityArray("x.ent", ents);
//  ^^^^^^^^^^^^^^ support.function
    sef::util::scheduleGlobalEvent(3000, function() { login::suspect("dfmd5\n\q"); }, false);
//  ^^^ variable.namespace
//             ^^^^^^^^^^^^^^^^^^ variable.function
//                                       ^^^^^^^^ keyword.declaration.function.anonymous
//                                                                         ^^ constant.character.escape
//                                                                           ^^ invalid.illegal.escape
    uint b = 0b1010 + KS_HIT + PI;
//           ^^^^^^ constant.numeric.integer.binary
//                    ^^^^^^ support.constant
//                             ^^ support.constant
    const ::string RUNEFORGE = "runeforge";
//                 ^^^^^^^^^ constant.other
    const ::string RIDE_THE_WIND = "ride-the-wind";
//                 ^^^^^^^^^^^^^ constant.other
    T x;
//  ^ - constant
    return ents;
}

const int x = foo(1);
// <- storage.modifier
//            ^^^ variable.function - entity
