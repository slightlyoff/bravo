// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bravo/bindings/v8_opengl_es2.h"

#include "base/strings/stringprintf.h"
#include "bravo/bindings/util.h"
#include "bravo/plugin/instance.h"
#include "bravo/plugin/ppb.h"
#include "v8/include/v8.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// See also:
//  https://code.google.com/p/chromium/codesearch#chromium/src/third_party/WebKit/Source/core/html/canvas/WebGLRenderingContext.cpp

namespace bravo {

namespace {

#define ARG_GLbitfield(arg) \
  (arg)->Int32Value()

#define ARG_GLenum(arg) \
  (arg)->Int32Value()

#define ARG_GLint(arg) \
  (arg)->Int32Value()

#define ARG_GLuint(arg) \
  (arg)->Uint32Value()

#define ARG_GLsizei(arg) \
  (arg)->Uint32Value()

#define ARG_GLfloat(arg) \
  (arg)->NumberValue()

#define ARG_GLclampf(arg) \
  (arg)->NumberValue()

#define ARG_GLboolean(arg) \
  (arg)->BooleanValue()

#define V8_BIND_0(interface, name)                                            \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    ppb.interface->name(                                                      \
      GetPPResource(info));                                                   \
  }

#define V8_BIND_1(interface, name, arg0)                                      \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 1)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]));                                                         \
  }

#define V8_BIND_2(interface, name, arg0, arg1)                                \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 2)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]));                                                         \
  }

#define V8_BIND_2_V(interface, name, arg0, arg1, type)                        \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 2)                                                    \
      return;                                                                 \
    type v = arg1(info[1]);                                                   \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      &v);                                                                    \
  }

#define V8_BIND_3(interface, name, arg0, arg1, arg2)                          \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 3)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      arg2(info[2]));                                                         \
  }

// V8_BIND_3_V(opengl_es2, Uniform4fv, ARG_GLint, ARG_GLsizei, ARG_GLfloat, GLfloat)
#define V8_BIND_3_V(interface, name, arg0, arg1, arg2, type)                  \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 3)                                                    \
      return;                                                                 \
    type v = arg2(info[2]);                                                   \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      &v);                                                                    \
  }

#define V8_BIND_4(interface, name, arg0, arg1, arg2, arg3)                    \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 4)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      arg2(info[2]),                                                          \
      arg3(info[3]));                                                         \
  }

#define V8_BIND_4_V(interface, name, arg0, arg1, arg2, arg3, type)            \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 4)                                                    \
      return;                                                                 \
    type v = arg3(info[3]);                                                   \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      arg2(info[2]),                                                          \
      &v);                                                                    \
  }

#define V8_BIND_5(interface, name, arg0, arg1, arg2, arg3, arg4)              \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 4)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      arg2(info[2]),                                                          \
      arg3(info[3]),                                                          \
      arg4(info[4]));                                                         \
  }

#define V8_BIND_8(interface, name, arg0, arg1, arg2, arg3,                    \
                                   arg4, arg5, arg6, arg7)                    \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 4)                                                    \
      return;                                                                 \
    ppb.interface->name(                                                      \
      GetPPResource(info),                                                    \
      arg0(info[0]),                                                          \
      arg1(info[1]),                                                          \
      arg2(info[2]),                                                          \
      arg3(info[3]),                                                          \
      arg4(info[4]),                                                          \
      arg5(info[5]),                                                          \
      arg6(info[6]),                                                          \
      arg7(info[7]));                                                         \
  }

#define V8_BIND_HANDLE_0(interface, name)                                     \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    info.GetReturnValue().Set(v8::Uint32::New(                                \
      ppb.interface->name(                                                    \
        GetPPResource(info)                                                   \
    )));                                                                      \
  }

#define V8_BIND_HANDLE_1(interface, name, arg0)                               \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 1)                                                    \
      return;                                                                 \
    info.GetReturnValue().Set(v8::Uint32::New(                                \
      ppb.interface->name(                                                    \
        GetPPResource(info),                                                  \
        arg0(info[0])                                                         \
    )));                                                                      \
  }

#define V8_BIND_HANDLE_BUFFER(interface, name, singular)                      \
  void singular(const v8::FunctionCallbackInfo<v8::Value>& info) {            \
    GLuint buffer = 0;                                                        \
    ppb.interface->name(GetPPResource(info), 1, &buffer);                     \
    info.GetReturnValue().Set(v8::Uint32::New(buffer));                       \
  }

#define V8_BIND_DELETE_BUFFER(interface, name, singular)                      \
  void singular(const v8::FunctionCallbackInfo<v8::Value>& info) {            \
    if (info.Length() < 1)                                                    \
      return;                                                                 \
    GLuint buffer = info[0]->Uint32Value();                                   \
    ppb.interface->name(GetPPResource(info), 1, &buffer);                     \
  }

#define V8_BIND_1_SHADER_STRING(interface, name, getter, type, arg0)          \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 1)                                                    \
      return;                                                                 \
                                                                              \
    GLsizei length = 0;                                                       \
    ppb.interface->getter(GetPPResource(info),                                \
                                arg0(info[0]),                                \
                                type,                                         \
                                &length);                                     \
    char buffer[length];                                                      \
    GLsizei returnedLength = 0;                                               \
    ppb.interface->name(GetPPResource(info),                                  \
                                     arg0(info[0]),                           \
                                     length,                                  \
                                     &returnedLength,                         \
                                     buffer);                                 \
    info.GetReturnValue().Set(v8::String::New(buffer, returnedLength));       \
  }


#define V8_BIND_UNIFORM_MATRIX(interface, name, v8type, typeTest, glType)     \
  void name(const v8::FunctionCallbackInfo<v8::Value>& info) {                \
    if (info.Length() < 3)                                                    \
      return;                                                                 \
                                                                              \
    /* TODO: handle regular arrays like WebGL does */                         \
    if (!info[2]->typeTest())                                                 \
      return;                                                                 \
                                                                              \
    GLint location = ARG_GLint(info[0]);                                      \
    GLboolean transpose =  ARG_GLboolean(info[1]);                            \
    v8::Handle<v8::v8type> arr = v8::Handle<v8::v8type>::Cast(info[2]);       \
    /* FIXME: LEAKS EXTERNALIZED */                                           \
    void * data = arr->Buffer()->Externalize().Data();                        \
    ppb.opengl_es2->UniformMatrix4fv(                                         \
        GetPPResource(info),                                                  \
        location,                                                             \
        1,                                                                    \
        transpose,                                                            \
        reinterpret_cast<const glType *>(data));                              \
  }


V8_BIND_1(opengl_es2, ActiveTexture, ARG_GLenum)
V8_BIND_2(opengl_es2, AttachShader, ARG_GLuint, ARG_GLuint)
  // void (*BindAttribLocation)(
  //     PP_Resource context, GLuint program, GLuint index, const char* name);
V8_BIND_2(opengl_es2, BindBuffer, ARG_GLenum, ARG_GLuint)
V8_BIND_2(opengl_es2, BindFramebuffer, ARG_GLenum, ARG_GLuint)
V8_BIND_2(opengl_es2, BindRenderbuffer, ARG_GLenum, ARG_GLuint)
V8_BIND_2(opengl_es2, BindTexture, ARG_GLenum, ARG_GLuint)
V8_BIND_4(opengl_es2, BlendColor,
    ARG_GLclampf, ARG_GLclampf, ARG_GLclampf, ARG_GLclampf)
V8_BIND_1(opengl_es2, BlendEquation, ARG_GLenum)
V8_BIND_2(opengl_es2, BlendEquationSeparate, ARG_GLenum, ARG_GLenum)
V8_BIND_2(opengl_es2, BlendFunc, ARG_GLenum, ARG_GLenum)
V8_BIND_4(opengl_es2, BlendFuncSeparate,
    ARG_GLenum, ARG_GLenum, ARG_GLenum, ARG_GLenum)

void BufferData(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 3)
    return;
  GLenum target = info[0]->Int32Value();
  GLenum usage = info[2]->Int32Value();
  /*
  void bufferData(GLenum target, GLsizeiptr size, GLenum usage) (OpenGL ES 2.0 §2.9, man page)
    Set the size of the currently bound WebGLBuffer object for the passed target. The buffer is initialized to 0.
  */
  if (info[1]->IsNumber()) {
    ppb.opengl_es2->BufferData(GetPPResource(info),
                               target,
                               info[1]->Int32Value(),
                               0,
                               usage);
    return;
  }
  /*
  void bufferData(GLenum target, ArrayBufferView data, GLenum usage)
  */
  if (info[1]->IsArrayBuffer()) {
    v8::Handle<v8::ArrayBuffer> buffer = v8::Handle<v8::ArrayBuffer>::Cast(info[1]);
    //                           (GLsizeiptr)buffer->ByteLength(),
    ppb.opengl_es2->BufferData(GetPPResource(info),
                               target,
                               buffer->ByteLength(),
                               buffer->Externalize().Data(),
                               usage);
  }
  /*
  void bufferData(GLenum target, ArrayBuffer? data, GLenum usage) (OpenGL ES 2.0 §2.9, man page)
    Set the size of the currently bound WebGLBuffer object for the passed target to the size of the passed data, then write the contents of data to the buffer object.
  */
  if (info[1]->IsArrayBufferView()) {
    v8::Handle<v8::ArrayBufferView> view =
        v8::Handle<v8::ArrayBufferView>::Cast(info[1]);
    ppb.opengl_es2->BufferData(GetPPResource(info),
                               target,
                               view->ByteLength(),
                               view->BaseAddress(),
                               usage);
  }
  /*
  TODO(slightlyoff):
    If the passed data is null then an INVALID_VALUE error is generated.
  */
}
  // void (*BufferSubData)(
  //     PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
  //     const void* data);
V8_BIND_1(opengl_es2, CheckFramebufferStatus, ARG_GLenum)
V8_BIND_1(opengl_es2, Clear, ARG_GLbitfield)
V8_BIND_4(opengl_es2, ClearColor,
    ARG_GLclampf, ARG_GLclampf, ARG_GLclampf, ARG_GLclampf)
V8_BIND_1(opengl_es2, ClearDepthf, ARG_GLclampf)
V8_BIND_1(opengl_es2, ClearStencil, ARG_GLint)
V8_BIND_4(opengl_es2, ColorMask,
    ARG_GLboolean, ARG_GLboolean, ARG_GLboolean, ARG_GLboolean)
V8_BIND_1(opengl_es2, CompileShader, ARG_GLuint)
  // void (*CompressedTexImage2D)(
  //     PP_Resource context, GLenum target, GLint level, GLenum internalformat,
  //     GLsizei width, GLsizei height, GLint border, GLsizei imageSize,
  //     const void* data);
V8_BIND_8(opengl_es2, CopyTexImage2D,
    ARG_GLenum, ARG_GLint, ARG_GLenum, ARG_GLint,
    ARG_GLint, ARG_GLsizei, ARG_GLsizei, ARG_GLint)
V8_BIND_8(opengl_es2, CopyTexSubImage2D,
    ARG_GLenum, ARG_GLint, ARG_GLint, ARG_GLint,
    ARG_GLint, ARG_GLint, ARG_GLsizei, ARG_GLsizei)
V8_BIND_HANDLE_BUFFER(opengl_es2, GenBuffers, CreateBuffer)
V8_BIND_HANDLE_BUFFER(opengl_es2, GenFramebuffers, CreateFramebuffer)
V8_BIND_HANDLE_BUFFER(opengl_es2, GenRenderbuffers, CreateRenderbuffer)
V8_BIND_HANDLE_BUFFER(opengl_es2, GenTextures, CreateTexture)
V8_BIND_HANDLE_0(opengl_es2, CreateProgram)
V8_BIND_HANDLE_1(opengl_es2, CreateShader, ARG_GLenum)
V8_BIND_1(opengl_es2, CullFace, ARG_GLenum)
V8_BIND_DELETE_BUFFER(opengl_es2, DeleteBuffers, DeleteBuffer)
V8_BIND_DELETE_BUFFER(opengl_es2, DeleteFramebuffers, DeleteFramebuffer)
V8_BIND_1(opengl_es2, DeleteProgram, ARG_GLuint)
V8_BIND_DELETE_BUFFER(opengl_es2, DeleteRenderbuffers, DeleteRenderbuffer)
V8_BIND_1(opengl_es2, DeleteShader, ARG_GLuint)
V8_BIND_DELETE_BUFFER(opengl_es2, DeleteTextures, DeleteTexture)
V8_BIND_1(opengl_es2, DepthFunc, ARG_GLenum)
V8_BIND_1(opengl_es2, DepthMask, ARG_GLboolean)
V8_BIND_2(opengl_es2, DepthRangef, ARG_GLclampf, ARG_GLclampf)
V8_BIND_2(opengl_es2, DetachShader, ARG_GLuint, ARG_GLuint)
V8_BIND_1(opengl_es2, Disable, ARG_GLenum)
V8_BIND_1(opengl_es2, DisableVertexAttribArray, ARG_GLuint)
V8_BIND_3(opengl_es2, DrawArrays, ARG_GLenum, ARG_GLint, ARG_GLsizei)
  // void (*DrawElements)(
  //     PP_Resource context, GLenum mode, GLsizei count, GLenum type,
  //     const void* indices);
void DrawElements(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 4)
    return;

  // GLint theBufferFromBefore;
  // ppb.opengl_es2->GetIntegerv(GetPPResource(info), GL_ELEMENT_ARRAY_BUFFER_BINDING, &theBufferFromBefore);

  // Log("the buffer from before:");
  // Log(theBufferFromBefore);
  // ppb.opengl_es2->Get(GL_ELEMENT_ARRAY_BUFFER, &theBufferFromBefore);

  GLenum mode = ARG_GLenum(info[0]);
  GLsizei count = ARG_GLsizei(info[1]);
  GLenum elementType = ARG_GLenum(info[2]);
  GLint indices = ARG_GLint(info[3]);
  // void* indices = theBufferFromBefore + ARG_GLint(info[3]);
  // Log(reinterpret_cast<int32_t>(indices));

  ppb.opengl_es2->DrawElements(GetPPResource(info),
                               mode,
                               count,
                               elementType,
                               reinterpret_cast<void *>(indices));
}

V8_BIND_1(opengl_es2, Enable, ARG_GLenum)
V8_BIND_1(opengl_es2, EnableVertexAttribArray, ARG_GLuint)
V8_BIND_0(opengl_es2, Finish)
V8_BIND_0(opengl_es2, Flush)
V8_BIND_4(opengl_es2, FramebufferRenderbuffer,
    ARG_GLenum, ARG_GLenum, ARG_GLenum, ARG_GLuint)
V8_BIND_5(opengl_es2, FramebufferTexture2D,
    ARG_GLenum, ARG_GLenum, ARG_GLenum, ARG_GLuint, ARG_GLint)
V8_BIND_1(opengl_es2, FrontFace, ARG_GLenum)
V8_BIND_1(opengl_es2, GenerateMipmap, ARG_GLenum)
  // void (*GetActiveAttrib)(
  //     PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
  //     GLsizei* length, GLint* size, GLenum* type, char* name);
  // void (*GetActiveUniform)(
  //     PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
  //     GLsizei* length, GLint* size, GLenum* type, char* name);
  // void (*GetAttachedShaders)(
  //     PP_Resource context, GLuint program, GLsizei maxcount, GLsizei* count,
  //     GLuint* shaders);
void GetAttribLocation(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  if (!info[0]->IsNumber())
    return;

  if (!info[1]->IsString())
    return;

  v8::String::Utf8Value name(info[1]);
  const char* str = *name;

  GLint value = ppb.opengl_es2->GetAttribLocation(GetPPResource(info),
                                                  ARG_GLint(info[0]),
                                                  str);
  info.GetReturnValue().Set(v8::Uint32::New(value));
}

  // void (*GetBooleanv)(PP_Resource context, GLenum pname, GLboolean* params);
  // void (*GetBufferParameteriv)(
  //     PP_Resource context, GLenum target, GLenum pname, GLint* params);
V8_BIND_HANDLE_0(opengl_es2, GetError)
  // void (*GetFloatv)(PP_Resource context, GLenum pname, GLfloat* params);
  // void (*GetFramebufferAttachmentParameteriv)(
  //     PP_Resource context, GLenum target, GLenum attachment, GLenum pname,
  //     GLint* params);
  // void (*GetIntegerv)(PP_Resource context, GLenum pname, GLint* params);
void GetProgramParameter(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  GLenum name = ARG_GLenum(info[1]);
  GLint value;
  ppb.opengl_es2->GetProgramiv(GetPPResource(info),
                              ARG_GLuint(info[0]),
                              name,
                              &value);

  if (name == GL_DELETE_STATUS ||
      name == GL_LINK_STATUS ||
      name == GL_VALIDATE_STATUS) {
    info.GetReturnValue().Set(v8::Boolean::New(value));
    return;
  }

  if (name == GL_ATTACHED_SHADERS ||
      name == GL_ACTIVE_ATTRIBUTES ||
      name == GL_ACTIVE_UNIFORMS) {
    info.GetReturnValue().Set(v8::Uint32::New(value));
    return;
  }
  info.GetReturnValue().Set(v8::Null());
}

  V8_BIND_1_SHADER_STRING(opengl_es2, GetProgramInfoLog, GetProgramiv, GL_INFO_LOG_LENGTH, ARG_GLuint)
  // void (*GetRenderbufferParameteriv)(
  //     PP_Resource context, GLenum target, GLenum pname, GLint* params);
  // void (*GetShaderiv)(
  //     PP_Resource context, GLuint shader, GLenum pname, GLint* params);
void GetShaderParameter(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;
  GLenum name = ARG_GLenum(info[1]);
  GLint value;
  ppb.opengl_es2->GetShaderiv(GetPPResource(info),
                              ARG_GLuint(info[0]),
                              name,
                              &value);

  if (name == GL_DELETE_STATUS ||
      name == GL_COMPILE_STATUS) {
    info.GetReturnValue().Set(v8::Boolean::New(value));
    return;
  }

  if (name == GL_SHADER_TYPE) {
    info.GetReturnValue().Set(v8::Uint32::New(value));
    return;
  }
  info.GetReturnValue().Set(v8::Null());
}

V8_BIND_1_SHADER_STRING(opengl_es2, GetShaderInfoLog, GetShaderiv, GL_INFO_LOG_LENGTH, ARG_GLuint)
  // void (*GetShaderPrecisionFormat)(
  //     PP_Resource context, GLenum shadertype, GLenum precisiontype,
  //     GLint* range, GLint* precision);
V8_BIND_1_SHADER_STRING(opengl_es2, GetShaderSource, GetShaderiv, GL_SHADER_SOURCE_LENGTH, ARG_GLuint)
  // const GLubyte* (*GetString)(PP_Resource context, GLenum name);
  // void (*GetTexParameterfv)(
  //     PP_Resource context, GLenum target, GLenum pname, GLfloat* params);
  // void (*GetTexParameteriv)(
  //     PP_Resource context, GLenum target, GLenum pname, GLint* params);
  // void (*GetUniformfv)(
  //     PP_Resource context, GLuint program, GLint location, GLfloat* params);
  // void (*GetUniformiv)(
  //     PP_Resource context, GLuint program, GLint location, GLint* params);

void GetUniformLocation(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;

  if (!info[0]->IsNumber())
    return;

  if (!info[1]->IsString())
    return;

  v8::String::Utf8Value name(info[1]);
  const char* str = *name;

  GLint value = ppb.opengl_es2->GetUniformLocation(GetPPResource(info),
                                                   ARG_GLint(info[0]),
                                                   str);
  info.GetReturnValue().Set(v8::Uint32::New(value));
}
  // void (*GetVertexAttribfv)(
  //     PP_Resource context, GLuint index, GLenum pname, GLfloat* params);
  // void (*GetVertexAttribiv)(
  //     PP_Resource context, GLuint index, GLenum pname, GLint* params);
  // void (*GetVertexAttribPointerv)(
  //     PP_Resource context, GLuint index, GLenum pname, void** pointer);
V8_BIND_2(opengl_es2, Hint, ARG_GLenum, ARG_GLenum)
  // GLboolean (*IsBuffer)(PP_Resource context, GLuint buffer);
  // GLboolean (*IsEnabled)(PP_Resource context, GLenum cap);
  // GLboolean (*IsFramebuffer)(PP_Resource context, GLuint framebuffer);
  // GLboolean (*IsProgram)(PP_Resource context, GLuint program);
  // GLboolean (*IsRenderbuffer)(PP_Resource context, GLuint renderbuffer);
  // GLboolean (*IsShader)(PP_Resource context, GLuint shader);
  // GLboolean (*IsTexture)(PP_Resource context, GLuint texture);
V8_BIND_1(opengl_es2, LineWidth, ARG_GLfloat)
V8_BIND_1(opengl_es2, LinkProgram, ARG_GLuint)
V8_BIND_2(opengl_es2, PixelStorei, ARG_GLenum, ARG_GLint)
V8_BIND_2(opengl_es2, PolygonOffset, ARG_GLfloat, ARG_GLfloat)
  // void (*ReadPixels)(
  //     PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
  //     GLenum format, GLenum type, void* pixels);
V8_BIND_0(opengl_es2, ReleaseShaderCompiler)
V8_BIND_4(opengl_es2, RenderbufferStorage,
    ARG_GLenum, ARG_GLenum, ARG_GLsizei, ARG_GLsizei)
V8_BIND_2(opengl_es2, SampleCoverage, ARG_GLclampf, ARG_GLboolean)
V8_BIND_4(opengl_es2, Scissor,
    ARG_GLint, ARG_GLint, ARG_GLsizei, ARG_GLsizei)
  // void (*ShaderBinary)(
  //     PP_Resource context, GLsizei n, const GLuint* shaders,
  //     GLenum binaryformat, const void* binary, GLsizei length);

// Goddamn this API sucks for JS. Handles? For serious?
void ShaderSource(const v8::FunctionCallbackInfo<v8::Value>& info) {
  // void (*ShaderSource)(
  //     PP_Resource context, GLuint shader, GLsizei count, const char** str,
  //     const GLint* length);
  if (info.Length() < 2)
    return;

  if (!info[1]->IsString())
    return;

  v8::String::Utf8Value shaderSource(info[1]);
  const char* str = *shaderSource;
  int length = shaderSource.length();

  ppb.opengl_es2->ShaderSource(
    GetPPResource(info),
    ARG_GLint(info[0]),
    1,
    &str, // string
    &length);
}

V8_BIND_3(opengl_es2, StencilFunc, ARG_GLenum, ARG_GLint, ARG_GLuint)
V8_BIND_4(opengl_es2, StencilFuncSeparate,
    ARG_GLenum, ARG_GLenum, ARG_GLint, ARG_GLuint)
V8_BIND_1(opengl_es2, StencilMask, ARG_GLuint)
V8_BIND_2(opengl_es2, StencilMaskSeparate, ARG_GLenum, ARG_GLuint)
V8_BIND_3(opengl_es2, StencilOp, ARG_GLenum, ARG_GLenum, ARG_GLenum)
V8_BIND_4(opengl_es2, StencilOpSeparate,
    ARG_GLenum, ARG_GLenum, ARG_GLenum, ARG_GLenum)
  // void (*TexImage2D)(
  //     PP_Resource context, GLenum target, GLint level, GLint internalformat,
  //     GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
  //     const void* pixels);
V8_BIND_3(opengl_es2, TexParameterf, ARG_GLenum, ARG_GLenum, ARG_GLfloat)
  // void (*TexParameterfv)(
  //     PP_Resource context, GLenum target, GLenum pname, const GLfloat* params);
V8_BIND_3(opengl_es2, TexParameteri, ARG_GLenum, ARG_GLenum, ARG_GLint)
  // void (*TexParameteriv)(
  //     PP_Resource context, GLenum target, GLenum pname, const GLint* params);
  // void (*TexSubImage2D)(
  //     PP_Resource context, GLenum target, GLint level, GLint xoffset,
  //     GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
  //     const void* pixels);
V8_BIND_2(opengl_es2, Uniform1f, ARG_GLint, ARG_GLfloat)
V8_BIND_3_V(opengl_es2, Uniform1fv, ARG_GLint, ARG_GLsizei, ARG_GLfloat,
    GLfloat)
V8_BIND_2(opengl_es2, Uniform1i, ARG_GLint, ARG_GLint)
V8_BIND_3_V(opengl_es2, Uniform1iv, ARG_GLint, ARG_GLsizei, ARG_GLint, GLint)
V8_BIND_3(opengl_es2, Uniform2f, ARG_GLint, ARG_GLfloat, ARG_GLfloat)
V8_BIND_3_V(opengl_es2, Uniform2fv, ARG_GLint, ARG_GLsizei, ARG_GLfloat,
    GLfloat)
V8_BIND_3(opengl_es2, Uniform2i, ARG_GLint, ARG_GLint, ARG_GLint)
V8_BIND_3_V(opengl_es2, Uniform2iv, ARG_GLint, ARG_GLsizei, ARG_GLint, GLint)
V8_BIND_4(opengl_es2, Uniform3f,
    ARG_GLint, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat)
V8_BIND_3_V(opengl_es2, Uniform3fv, ARG_GLint, ARG_GLsizei, ARG_GLfloat,
    GLfloat)
V8_BIND_4(opengl_es2, Uniform3i, ARG_GLint, ARG_GLint, ARG_GLint, ARG_GLint)
V8_BIND_3_V(opengl_es2, Uniform3iv, ARG_GLint, ARG_GLsizei, ARG_GLint, GLint)

//V8_BIND_5(opengl_es2, Uniform4f,
//    ARG_GLint, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat)
void Uniform4f(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 4)
    return;
  GLint location = ARG_GLint(info[0]);
  GLfloat f1 = ARG_GLfloat(info[1]);
  GLfloat f2 = ARG_GLfloat(info[2]);
  GLfloat f3 = ARG_GLfloat(info[3]);
  GLfloat f4 = ARG_GLfloat(info[4]);
  ppb.opengl_es2->Uniform4f(GetPPResource(info), location, f1, f2, f3, f4);
}

void Uniform4fv(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2)
    return;
  // TODO: handle regular arrays like WebGL does
  if (!info[1]->IsFloat32Array())
    return;

  GLint location = ARG_GLint(info[0]);
  v8::Handle<v8::Float32Array> arr =
      v8::Handle<v8::Float32Array>::Cast(info[1]);
  // Log(arr->Length());
  ppb.opengl_es2->Uniform4fv(GetPPResource(info),
                             location,
                             1,
                             reinterpret_cast<const GLfloat *>(arr->Buffer()->Externalize().Data()));
}

V8_BIND_5(opengl_es2, Uniform4i,
    ARG_GLint, ARG_GLint, ARG_GLint, ARG_GLint, ARG_GLint)
V8_BIND_3_V(opengl_es2, Uniform4iv, ARG_GLint, ARG_GLsizei, ARG_GLint, GLint)
V8_BIND_UNIFORM_MATRIX(opengl_es2, UniformMatrix2fv, Float32Array, IsFloat32Array, GLfloat)
V8_BIND_UNIFORM_MATRIX(opengl_es2, UniformMatrix3fv, Float32Array, IsFloat32Array, GLfloat)
V8_BIND_UNIFORM_MATRIX(opengl_es2, UniformMatrix4fv, Float32Array, IsFloat32Array, GLfloat)
V8_BIND_1(opengl_es2, UseProgram, ARG_GLuint)
V8_BIND_1(opengl_es2, ValidateProgram, ARG_GLuint)
V8_BIND_2(opengl_es2, VertexAttrib1f, ARG_GLuint, ARG_GLfloat)
V8_BIND_2_V(opengl_es2, VertexAttrib1fv, ARG_GLuint, ARG_GLfloat, GLfloat)
V8_BIND_3(opengl_es2, VertexAttrib2f, ARG_GLuint, ARG_GLfloat, ARG_GLfloat)
V8_BIND_2_V(opengl_es2, VertexAttrib2fv, ARG_GLuint, ARG_GLfloat, GLfloat)
V8_BIND_4(opengl_es2, VertexAttrib3f,
    ARG_GLuint, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat)
V8_BIND_2_V(opengl_es2, VertexAttrib3fv, ARG_GLuint, ARG_GLfloat, GLfloat)
V8_BIND_5(opengl_es2, VertexAttrib4f,
    ARG_GLuint, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat, ARG_GLfloat)
V8_BIND_2_V(opengl_es2, VertexAttrib4fv, ARG_GLuint, ARG_GLfloat, GLfloat)

void VertexAttribPointer(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 5)
    return;

  ppb.opengl_es2->VertexAttribPointer(
    GetPPResource(info),
    ARG_GLuint(info[0]),
    ARG_GLint(info[1]),
    ARG_GLenum(info[2]),
    ARG_GLboolean(info[3]),
    ARG_GLsizei(info[4]),
    reinterpret_cast<void*>(static_cast<intptr_t>(ARG_GLint(info[4]))));
}

V8_BIND_4(opengl_es2, Viewport,
    ARG_GLint, ARG_GLint, ARG_GLsizei, ARG_GLsizei)

static const size_t kMethodCount = 107;

static const MethodConfiguration g_methods[kMethodCount] = {
  { "activeTexture", ActiveTexture },
  { "attachShader", AttachShader },
  { "bindBuffer", BindBuffer },
  { "bindFramebuffer", BindFramebuffer },
  { "bindRenderbuffer", BindRenderbuffer },
  { "bindTexture", BindTexture },
  { "blendColor", BlendColor },
  { "blendEquation", BlendEquation },
  { "blendEquationSeparate", BlendEquationSeparate },
  { "blendFunc", BlendFunc },
  { "blendFuncSeparate", BlendFuncSeparate },
  { "bufferData", BufferData },
  { "checkFramebufferStatus", CheckFramebufferStatus },
  { "clear", Clear },
  { "clearColor", ClearColor },
  { "clearDepth", ClearDepthf },
  { "clearStencil", ClearStencil },
  { "colorMask", ColorMask },
  { "compileShader", CompileShader },
  { "copyTexImage2D", CopyTexImage2D },
  { "copyTexSubImage2D", CopyTexSubImage2D },
  { "createBuffer", CreateBuffer },
  { "createFramebuffer", CreateFramebuffer },
  { "createProgram", CreateProgram },
  { "createRenderbuffer", CreateRenderbuffer },
  { "createShader", CreateShader },
  { "createTexture", CreateTexture },
  { "cullFace", CullFace },
  { "deleteBuffer", DeleteBuffer },
  { "deleteFramebuffer", DeleteFramebuffer },
  { "deleteProgram", DeleteProgram },
  { "deleteRenderbuffer", DeleteRenderbuffer },
  { "deleteShader", DeleteShader },
  { "deleteTexture", DeleteTexture },
  { "depthFunc", DepthFunc },
  { "depthMask", DepthMask },
  { "depthRangef", DepthRangef },
  { "detachShader", DetachShader },
  { "disable", Disable },
  { "disableVertexAttribArray", DisableVertexAttribArray },
  { "drawArrays", DrawArrays },
  { "drawElements", DrawElements },
  { "enable", Enable },
  { "enableVertexAttribArray", EnableVertexAttribArray },
  { "finish", Finish },
  { "flush", Flush },
  { "framebufferRenderbuffer", FramebufferRenderbuffer },
  { "framebufferTexture2D", FramebufferTexture2D },
  { "frontFace", FrontFace },
  { "generateMipmap", GenerateMipmap },
  { "getAttribLocation", GetAttribLocation },
  { "getError", GetError },
  { "getProgramInfoLog", GetProgramInfoLog },
  { "getProgramParameter", GetProgramParameter },
  { "getShaderInfoLog", GetShaderInfoLog },
  { "getShaderParameter", GetShaderParameter },
  { "getShaderSource", GetShaderSource },
  { "getUniformLocation", GetUniformLocation },
  { "hint", Hint },
  { "lineWidth", LineWidth },
  { "linkProgram", LinkProgram },
  { "pixelStorei", PixelStorei },
  { "polygonOffset", PolygonOffset },
  { "releaseShaderCompiler", ReleaseShaderCompiler },
  { "renderbufferStorage", RenderbufferStorage },
  { "sampleCoverage", SampleCoverage },
  { "scissor", Scissor },
  { "shaderSource", ShaderSource },
  { "stencilFunc", StencilFunc },
  { "stencilFuncSeparate", StencilFuncSeparate },
  { "stencilMask", StencilMask },
  { "stencilMaskSeparate", StencilMaskSeparate },
  { "stencilOp", StencilOp },
  { "stencilOpSeparate", StencilOpSeparate },
  { "texParameterf", TexParameterf },
  { "texParameteri", TexParameteri },
  { "uniform1f", Uniform1f },
  { "uniform1fv", Uniform1fv },
  { "uniform1i", Uniform1i },
  { "uniform1iv", Uniform1iv },
  { "uniform2f", Uniform2f },
  { "uniform2fv", Uniform2fv },
  { "uniform2i", Uniform2i },
  { "uniform2iv", Uniform2iv },
  { "uniform3f", Uniform3f },
  { "uniform3fv", Uniform3fv },
  { "uniform3i", Uniform3i },
  { "uniform3iv", Uniform3iv },
  { "uniform4f", Uniform4f },
  { "uniform4fv", Uniform4fv },
  { "uniform4i", Uniform4i },
  { "uniform4iv", Uniform4iv },
  { "uniformMatrix2fv", UniformMatrix2fv },
  { "uniformMatrix3fv", UniformMatrix3fv },
  { "uniformMatrix4fv", UniformMatrix4fv },
  { "useProgram", UseProgram },
  { "validateProgram", ValidateProgram },
  { "vertexAttrib1f", VertexAttrib1f },
  { "vertexAttrib1fv", VertexAttrib1fv },
  { "vertexAttrib2f", VertexAttrib2f },
  { "vertexAttrib2fv", VertexAttrib2fv },
  { "vertexAttrib3f", VertexAttrib3f },
  { "vertexAttrib3fv", VertexAttrib3fv },
  { "vertexAttrib4f", VertexAttrib4f },
  { "vertexAttrib4fv", VertexAttrib4fv },
  { "vertexAttribPointer", VertexAttribPointer },
  { "viewport", Viewport },
};

static const size_t kConstantCount = 301;

static const ConstantConfiguration g_constants[kConstantCount] = {
  { "DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT },
  { "STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT },
  { "COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT },
  { "FALSE", GL_FALSE },
  { "TRUE", GL_TRUE },
  { "POINTS", GL_POINTS },
  { "LINES", GL_LINES },
  { "LINE_LOOP", GL_LINE_LOOP },
  { "LINE_STRIP", GL_LINE_STRIP },
  { "TRIANGLES", GL_TRIANGLES },
  { "TRIANGLE_STRIP", GL_TRIANGLE_STRIP },
  { "TRIANGLE_FAN", GL_TRIANGLE_FAN },
  { "ZERO", GL_ZERO },
  { "ONE", GL_ONE },
  { "SRC_COLOR", GL_SRC_COLOR },
  { "ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR },
  { "SRC_ALPHA", GL_SRC_ALPHA },
  { "ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA },
  { "DST_ALPHA", GL_DST_ALPHA },
  { "ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA },
  { "DST_COLOR", GL_DST_COLOR },
  { "ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR },
  { "SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE },
  { "FUNC_ADD", GL_FUNC_ADD },
  { "BLEND_EQUATION", GL_BLEND_EQUATION },
  { "BLEND_EQUATION_RGB", GL_BLEND_EQUATION_RGB },
  { "BLEND_EQUATION_ALPHA", GL_BLEND_EQUATION_ALPHA },
  { "FUNC_SUBTRACT", GL_FUNC_SUBTRACT },
  { "FUNC_REVERSE_SUBTRACT", GL_FUNC_REVERSE_SUBTRACT },
  { "BLEND_DST_RGB", GL_BLEND_DST_RGB },
  { "BLEND_SRC_RGB", GL_BLEND_SRC_RGB },
  { "BLEND_DST_ALPHA", GL_BLEND_DST_ALPHA },
  { "BLEND_SRC_ALPHA", GL_BLEND_SRC_ALPHA },
  { "CONSTANT_COLOR", GL_CONSTANT_COLOR },
  { "ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR },
  { "CONSTANT_ALPHA", GL_CONSTANT_ALPHA },
  { "ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA },
  { "BLEND_COLOR", GL_BLEND_COLOR },
  { "ARRAY_BUFFER", GL_ARRAY_BUFFER },
  { "ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER },
  { "ARRAY_BUFFER_BINDING", GL_ARRAY_BUFFER_BINDING },
  { "ELEMENT_ARRAY_BUFFER_BINDING", GL_ELEMENT_ARRAY_BUFFER_BINDING },
  { "STREAM_DRAW", GL_STREAM_DRAW },
  { "STATIC_DRAW", GL_STATIC_DRAW },
  { "DYNAMIC_DRAW", GL_DYNAMIC_DRAW },
  { "BUFFER_SIZE", GL_BUFFER_SIZE },
  { "BUFFER_USAGE", GL_BUFFER_USAGE },
  { "CURRENT_VERTEX_ATTRIB", GL_CURRENT_VERTEX_ATTRIB },
  { "FRONT", GL_FRONT },
  { "BACK", GL_BACK },
  { "FRONT_AND_BACK", GL_FRONT_AND_BACK },
  { "TEXTURE_2D", GL_TEXTURE_2D },
  { "CULL_FACE", GL_CULL_FACE },
  { "BLEND", GL_BLEND },
  { "DITHER", GL_DITHER },
  { "STENCIL_TEST", GL_STENCIL_TEST },
  { "DEPTH_TEST", GL_DEPTH_TEST },
  { "SCISSOR_TEST", GL_SCISSOR_TEST },
  { "POLYGON_OFFSET_FILL", GL_POLYGON_OFFSET_FILL },
  { "SAMPLE_ALPHA_TO_COVERAGE", GL_SAMPLE_ALPHA_TO_COVERAGE },
  { "SAMPLE_COVERAGE", GL_SAMPLE_COVERAGE },
  { "NO_ERROR", GL_NO_ERROR },
  { "INVALID_ENUM", GL_INVALID_ENUM },
  { "INVALID_VALUE", GL_INVALID_VALUE },
  { "INVALID_OPERATION", GL_INVALID_OPERATION },
  { "OUT_OF_MEMORY", GL_OUT_OF_MEMORY },
  { "CW", GL_CW },
  { "CCW", GL_CCW },
  { "LINE_WIDTH", GL_LINE_WIDTH },
  { "ALIASED_POINT_SIZE_RANGE", GL_ALIASED_POINT_SIZE_RANGE },
  { "ALIASED_LINE_WIDTH_RANGE", GL_ALIASED_LINE_WIDTH_RANGE },
  { "CULL_FACE_MODE", GL_CULL_FACE_MODE },
  { "FRONT_FACE", GL_FRONT_FACE },
  { "DEPTH_RANGE", GL_DEPTH_RANGE },
  { "DEPTH_WRITEMASK", GL_DEPTH_WRITEMASK },
  { "DEPTH_CLEAR_VALUE", GL_DEPTH_CLEAR_VALUE },
  { "DEPTH_FUNC", GL_DEPTH_FUNC },
  { "STENCIL_CLEAR_VALUE", GL_STENCIL_CLEAR_VALUE },
  { "STENCIL_FUNC", GL_STENCIL_FUNC },
  { "STENCIL_FAIL", GL_STENCIL_FAIL },
  { "STENCIL_PASS_DEPTH_FAIL", GL_STENCIL_PASS_DEPTH_FAIL },
  { "STENCIL_PASS_DEPTH_PASS", GL_STENCIL_PASS_DEPTH_PASS },
  { "STENCIL_REF", GL_STENCIL_REF },
  { "STENCIL_VALUE_MASK", GL_STENCIL_VALUE_MASK },
  { "STENCIL_WRITEMASK", GL_STENCIL_WRITEMASK },
  { "STENCIL_BACK_FUNC", GL_STENCIL_BACK_FUNC },
  { "STENCIL_BACK_FAIL", GL_STENCIL_BACK_FAIL },
  { "STENCIL_BACK_PASS_DEPTH_FAIL", GL_STENCIL_BACK_PASS_DEPTH_FAIL },
  { "STENCIL_BACK_PASS_DEPTH_PASS", GL_STENCIL_BACK_PASS_DEPTH_PASS },
  { "STENCIL_BACK_REF", GL_STENCIL_BACK_REF },
  { "STENCIL_BACK_VALUE_MASK", GL_STENCIL_BACK_VALUE_MASK },
  { "STENCIL_BACK_WRITEMASK", GL_STENCIL_BACK_WRITEMASK },
  { "VIEWPORT", GL_VIEWPORT },
  { "SCISSOR_BOX", GL_SCISSOR_BOX },
  { "COLOR_CLEAR_VALUE", GL_COLOR_CLEAR_VALUE },
  { "COLOR_WRITEMASK", GL_COLOR_WRITEMASK },
  { "UNPACK_ALIGNMENT", GL_UNPACK_ALIGNMENT },
  { "PACK_ALIGNMENT", GL_PACK_ALIGNMENT },
  { "MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE },
  { "MAX_VIEWPORT_DIMS", GL_MAX_VIEWPORT_DIMS },
  { "SUBPIXEL_BITS", GL_SUBPIXEL_BITS },
  { "RED_BITS", GL_RED_BITS },
  { "GREEN_BITS", GL_GREEN_BITS },
  { "BLUE_BITS", GL_BLUE_BITS },
  { "ALPHA_BITS", GL_ALPHA_BITS },
  { "DEPTH_BITS", GL_DEPTH_BITS },
  { "STENCIL_BITS", GL_STENCIL_BITS },
  { "POLYGON_OFFSET_UNITS", GL_POLYGON_OFFSET_UNITS },
  { "POLYGON_OFFSET_FACTOR", GL_POLYGON_OFFSET_FACTOR },
  { "TEXTURE_BINDING_2D", GL_TEXTURE_BINDING_2D },
  { "SAMPLE_BUFFERS", GL_SAMPLE_BUFFERS },
  { "SAMPLES", GL_SAMPLES },
  { "SAMPLE_COVERAGE_VALUE", GL_SAMPLE_COVERAGE_VALUE },
  { "SAMPLE_COVERAGE_INVERT", GL_SAMPLE_COVERAGE_INVERT },
  { "NUM_COMPRESSED_TEXTURE_FORMATS", GL_NUM_COMPRESSED_TEXTURE_FORMATS },
  { "COMPRESSED_TEXTURE_FORMATS", GL_COMPRESSED_TEXTURE_FORMATS },
  { "DONT_CARE", GL_DONT_CARE },
  { "FASTEST", GL_FASTEST },
  { "NICEST", GL_NICEST },
  { "GENERATE_MIPMAP_HINT", GL_GENERATE_MIPMAP_HINT },
  { "BYTE", GL_BYTE },
  { "UNSIGNED_BYTE", GL_UNSIGNED_BYTE },
  { "SHORT", GL_SHORT },
  { "UNSIGNED_SHORT", GL_UNSIGNED_SHORT },
  { "INT", GL_INT },
  { "UNSIGNED_INT", GL_UNSIGNED_INT },
  { "FLOAT", GL_FLOAT },
  { "FIXED", GL_FIXED },
  { "DEPTH_COMPONENT", GL_DEPTH_COMPONENT },
  { "ALPHA", GL_ALPHA },
  { "RGB", GL_RGB },
  { "RGBA", GL_RGBA },
  { "LUMINANCE", GL_LUMINANCE },
  { "LUMINANCE_ALPHA", GL_LUMINANCE_ALPHA },
  { "UNSIGNED_SHORT_4_4_4_4", GL_UNSIGNED_SHORT_4_4_4_4 },
  { "UNSIGNED_SHORT_5_5_5_1", GL_UNSIGNED_SHORT_5_5_5_1 },
  { "UNSIGNED_SHORT_5_6_5", GL_UNSIGNED_SHORT_5_6_5 },
  { "FRAGMENT_SHADER", GL_FRAGMENT_SHADER },
  { "VERTEX_SHADER", GL_VERTEX_SHADER },
  { "MAX_VERTEX_ATTRIBS", GL_MAX_VERTEX_ATTRIBS },
  { "MAX_VERTEX_UNIFORM_VECTORS", GL_MAX_VERTEX_UNIFORM_VECTORS },
  { "MAX_VARYING_VECTORS", GL_MAX_VARYING_VECTORS },
  { "MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS },
  { "MAX_VERTEX_TEXTURE_IMAGE_UNITS", GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS },
  { "MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS },
  { "MAX_FRAGMENT_UNIFORM_VECTORS", GL_MAX_FRAGMENT_UNIFORM_VECTORS },
  { "SHADER_TYPE", GL_SHADER_TYPE },
  { "DELETE_STATUS", GL_DELETE_STATUS },
  { "LINK_STATUS", GL_LINK_STATUS },
  { "VALIDATE_STATUS", GL_VALIDATE_STATUS },
  { "ATTACHED_SHADERS", GL_ATTACHED_SHADERS },
  { "ACTIVE_UNIFORMS", GL_ACTIVE_UNIFORMS },
  { "ACTIVE_UNIFORM_MAX_LENGTH", GL_ACTIVE_UNIFORM_MAX_LENGTH },
  { "ACTIVE_ATTRIBUTES", GL_ACTIVE_ATTRIBUTES },
  { "ACTIVE_ATTRIBUTE_MAX_LENGTH", GL_ACTIVE_ATTRIBUTE_MAX_LENGTH },
  { "SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION },
  { "CURRENT_PROGRAM", GL_CURRENT_PROGRAM },
  { "NEVER", GL_NEVER },
  { "LESS", GL_LESS },
  { "EQUAL", GL_EQUAL },
  { "LEQUAL", GL_LEQUAL },
  { "GREATER", GL_GREATER },
  { "NOTEQUAL", GL_NOTEQUAL },
  { "GEQUAL", GL_GEQUAL },
  { "ALWAYS", GL_ALWAYS },
  { "KEEP", GL_KEEP },
  { "REPLACE", GL_REPLACE },
  { "INCR", GL_INCR },
  { "DECR", GL_DECR },
  { "INVERT", GL_INVERT },
  { "INCR_WRAP", GL_INCR_WRAP },
  { "DECR_WRAP", GL_DECR_WRAP },
  { "VENDOR", GL_VENDOR },
  { "RENDERER", GL_RENDERER },
  { "VERSION", GL_VERSION },
  { "EXTENSIONS", GL_EXTENSIONS },
  { "NEAREST", GL_NEAREST },
  { "LINEAR", GL_LINEAR },
  { "NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST },
  { "LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST },
  { "NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR },
  { "LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR },
  { "TEXTURE_MAG_FILTER", GL_TEXTURE_MAG_FILTER },
  { "TEXTURE_MIN_FILTER", GL_TEXTURE_MIN_FILTER },
  { "TEXTURE_WRAP_S", GL_TEXTURE_WRAP_S },
  { "TEXTURE_WRAP_T", GL_TEXTURE_WRAP_T },
  { "TEXTURE", GL_TEXTURE },
  { "TEXTURE_CUBE_MAP", GL_TEXTURE_CUBE_MAP },
  { "TEXTURE_BINDING_CUBE_MAP", GL_TEXTURE_BINDING_CUBE_MAP },
  { "TEXTURE_CUBE_MAP_POSITIVE_X", GL_TEXTURE_CUBE_MAP_POSITIVE_X },
  { "TEXTURE_CUBE_MAP_NEGATIVE_X", GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
  { "TEXTURE_CUBE_MAP_POSITIVE_Y", GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
  { "TEXTURE_CUBE_MAP_NEGATIVE_Y", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
  { "TEXTURE_CUBE_MAP_POSITIVE_Z", GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
  { "TEXTURE_CUBE_MAP_NEGATIVE_Z", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z },
  { "MAX_CUBE_MAP_TEXTURE_SIZE", GL_MAX_CUBE_MAP_TEXTURE_SIZE },
  { "TEXTURE0", GL_TEXTURE0 },
  { "TEXTURE1", GL_TEXTURE1 },
  { "TEXTURE2", GL_TEXTURE2 },
  { "TEXTURE3", GL_TEXTURE3 },
  { "TEXTURE4", GL_TEXTURE4 },
  { "TEXTURE5", GL_TEXTURE5 },
  { "TEXTURE6", GL_TEXTURE6 },
  { "TEXTURE7", GL_TEXTURE7 },
  { "TEXTURE8", GL_TEXTURE8 },
  { "TEXTURE9", GL_TEXTURE9 },
  { "TEXTURE10", GL_TEXTURE10 },
  { "TEXTURE11", GL_TEXTURE11 },
  { "TEXTURE12", GL_TEXTURE12 },
  { "TEXTURE13", GL_TEXTURE13 },
  { "TEXTURE14", GL_TEXTURE14 },
  { "TEXTURE15", GL_TEXTURE15 },
  { "TEXTURE16", GL_TEXTURE16 },
  { "TEXTURE17", GL_TEXTURE17 },
  { "TEXTURE18", GL_TEXTURE18 },
  { "TEXTURE19", GL_TEXTURE19 },
  { "TEXTURE20", GL_TEXTURE20 },
  { "TEXTURE21", GL_TEXTURE21 },
  { "TEXTURE22", GL_TEXTURE22 },
  { "TEXTURE23", GL_TEXTURE23 },
  { "TEXTURE24", GL_TEXTURE24 },
  { "TEXTURE25", GL_TEXTURE25 },
  { "TEXTURE26", GL_TEXTURE26 },
  { "TEXTURE27", GL_TEXTURE27 },
  { "TEXTURE28", GL_TEXTURE28 },
  { "TEXTURE29", GL_TEXTURE29 },
  { "TEXTURE30", GL_TEXTURE30 },
  { "TEXTURE31", GL_TEXTURE31 },
  { "ACTIVE_TEXTURE", GL_ACTIVE_TEXTURE },
  { "REPEAT", GL_REPEAT },
  { "CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE },
  { "MIRRORED_REPEAT", GL_MIRRORED_REPEAT },
  { "FLOAT_VEC2", GL_FLOAT_VEC2 },
  { "FLOAT_VEC3", GL_FLOAT_VEC3 },
  { "FLOAT_VEC4", GL_FLOAT_VEC4 },
  { "INT_VEC2", GL_INT_VEC2 },
  { "INT_VEC3", GL_INT_VEC3 },
  { "INT_VEC4", GL_INT_VEC4 },
  { "BOOL", GL_BOOL },
  { "BOOL_VEC2", GL_BOOL_VEC2 },
  { "BOOL_VEC3", GL_BOOL_VEC3 },
  { "BOOL_VEC4", GL_BOOL_VEC4 },
  { "FLOAT_MAT2", GL_FLOAT_MAT2 },
  { "FLOAT_MAT3", GL_FLOAT_MAT3 },
  { "FLOAT_MAT4", GL_FLOAT_MAT4 },
  { "SAMPLER_2D", GL_SAMPLER_2D },
  { "SAMPLER_CUBE", GL_SAMPLER_CUBE },
  { "VERTEX_ATTRIB_ARRAY_ENABLED", GL_VERTEX_ATTRIB_ARRAY_ENABLED },
  { "VERTEX_ATTRIB_ARRAY_SIZE", GL_VERTEX_ATTRIB_ARRAY_SIZE },
  { "VERTEX_ATTRIB_ARRAY_STRIDE", GL_VERTEX_ATTRIB_ARRAY_STRIDE },
  { "VERTEX_ATTRIB_ARRAY_TYPE", GL_VERTEX_ATTRIB_ARRAY_TYPE },
  { "VERTEX_ATTRIB_ARRAY_NORMALIZED", GL_VERTEX_ATTRIB_ARRAY_NORMALIZED },
  { "VERTEX_ATTRIB_ARRAY_POINTER", GL_VERTEX_ATTRIB_ARRAY_POINTER },
  { "VERTEX_ATTRIB_ARRAY_BUFFER_BINDING", GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING },
  { "IMPLEMENTATION_COLOR_READ_TYPE", GL_IMPLEMENTATION_COLOR_READ_TYPE },
  { "IMPLEMENTATION_COLOR_READ_FORMAT", GL_IMPLEMENTATION_COLOR_READ_FORMAT },
  { "COMPILE_STATUS", GL_COMPILE_STATUS },
  { "INFO_LOG_LENGTH", GL_INFO_LOG_LENGTH },
  { "SHADER_SOURCE_LENGTH", GL_SHADER_SOURCE_LENGTH },
  { "SHADER_COMPILER", GL_SHADER_COMPILER },
  { "SHADER_BINARY_FORMATS", GL_SHADER_BINARY_FORMATS },
  { "NUM_SHADER_BINARY_FORMATS", GL_NUM_SHADER_BINARY_FORMATS },
  { "LOW_FLOAT", GL_LOW_FLOAT },
  { "MEDIUM_FLOAT", GL_MEDIUM_FLOAT },
  { "HIGH_FLOAT", GL_HIGH_FLOAT },
  { "LOW_INT", GL_LOW_INT },
  { "MEDIUM_INT", GL_MEDIUM_INT },
  { "HIGH_INT", GL_HIGH_INT },
  { "FRAMEBUFFER", GL_FRAMEBUFFER },
  { "RENDERBUFFER", GL_RENDERBUFFER },
  { "RGBA4", GL_RGBA4 },
  { "RGB5_A1", GL_RGB5_A1 },
  { "RGB565", GL_RGB565 },
  { "DEPTH_COMPONENT16", GL_DEPTH_COMPONENT16 },
  { "STENCIL_INDEX8", GL_STENCIL_INDEX8 },
  { "RENDERBUFFER_WIDTH", GL_RENDERBUFFER_WIDTH },
  { "RENDERBUFFER_HEIGHT", GL_RENDERBUFFER_HEIGHT },
  { "RENDERBUFFER_INTERNAL_FORMAT", GL_RENDERBUFFER_INTERNAL_FORMAT },
  { "RENDERBUFFER_RED_SIZE", GL_RENDERBUFFER_RED_SIZE },
  { "RENDERBUFFER_GREEN_SIZE", GL_RENDERBUFFER_GREEN_SIZE },
  { "RENDERBUFFER_BLUE_SIZE", GL_RENDERBUFFER_BLUE_SIZE },
  { "RENDERBUFFER_ALPHA_SIZE", GL_RENDERBUFFER_ALPHA_SIZE },
  { "RENDERBUFFER_DEPTH_SIZE", GL_RENDERBUFFER_DEPTH_SIZE },
  { "RENDERBUFFER_STENCIL_SIZE", GL_RENDERBUFFER_STENCIL_SIZE },
  { "FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE", GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE },
  { "FRAMEBUFFER_ATTACHMENT_OBJECT_NAME", GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME },
  { "FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL", GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL },
  { "FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE", GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE },
  { "COLOR_ATTACHMENT0", GL_COLOR_ATTACHMENT0 },
  { "DEPTH_ATTACHMENT", GL_DEPTH_ATTACHMENT },
  { "STENCIL_ATTACHMENT", GL_STENCIL_ATTACHMENT },
  { "NONE", GL_NONE },
  { "FRAMEBUFFER_COMPLETE", GL_FRAMEBUFFER_COMPLETE },
  { "FRAMEBUFFER_INCOMPLETE_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT },
  { "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT },
  { "FRAMEBUFFER_INCOMPLETE_DIMENSIONS", GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS },
  { "FRAMEBUFFER_UNSUPPORTED", GL_FRAMEBUFFER_UNSUPPORTED },
  { "FRAMEBUFFER_BINDING", GL_FRAMEBUFFER_BINDING },
  { "RENDERBUFFER_BINDING", GL_RENDERBUFFER_BINDING },
  { "MAX_RENDERBUFFER_SIZE", GL_MAX_RENDERBUFFER_SIZE },
  { "INVALID_FRAMEBUFFER_OPERATION", GL_INVALID_FRAMEBUFFER_OPERATION },
};

}

v8::Handle<v8::FunctionTemplate> CreateWebGLBindings() {
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
  v8::Handle<v8::ObjectTemplate> proto = templ->PrototypeTemplate();

  InstallConstants(proto, g_constants, kConstantCount);
  InstallMethods(proto, g_methods, kMethodCount);

  return handle_scope.Close(templ);
}

}
