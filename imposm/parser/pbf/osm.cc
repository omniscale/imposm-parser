// -*- C++ -*-
#include <Python.h>
#include <string>
#include <sstream>
#include "structmember.h"
#include "osm.pb.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>




static PyObject *
fastpb_convert5(::google::protobuf::int32 value)
{
    return PyLong_FromLong(value);
}

static PyObject *
fastpb_convert3(::google::protobuf::int64 value)
{
    return PyLong_FromLongLong(value);
}

static PyObject *
fastpb_convert18(::google::protobuf::int64 value)
{
    return PyLong_FromLongLong(value);
}

static PyObject *
fastpb_convert17(::google::protobuf::int32 value)
{
    return PyLong_FromLong(value);
}

static PyObject *
fastpb_convert13(::google::protobuf::uint32 value)
{
    return PyLong_FromUnsignedLong(value);
}

static PyObject *
fastpb_convert7(::google::protobuf::int32 value)
{
    return PyLong_FromLong(value);
}

static PyObject *
fastpb_convert4(::google::protobuf::uint64 value)
{
    return PyLong_FromUnsignedLong(value);
}

static PyObject *
fastpb_convert1(double value)
{
    return PyFloat_FromDouble(value);
}

static PyObject *
fastpb_convert2(float value)
{
   return PyFloat_FromDouble(value);
}

static PyObject *
fastpb_convert9(const ::std::string &value)
{
    return PyUnicode_Decode(value.data(), value.length(), "utf-8", NULL);
}

static PyObject *
fastpb_convert12(const ::std::string &value)
{
    return PyString_FromStringAndSize(value.data(), value.length());
}

static PyObject *
fastpb_convert8(bool value)
{
    return PyBool_FromLong(value ? 1 : 0);
}

static PyObject *
fastpb_convert14(int value)
{
    // TODO(robbyw): Check EnumName_IsValid(value)
    return PyInt_FromLong(value);
}





// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject BlobType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::Blob *protobuf;
  } Blob;

  void
  Blob_dealloc(Blob* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  Blob_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      Blob *self;

      self = (Blob *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::Blob();

      return (PyObject *)self;
  }

  PyObject *
  Blob_DebugString(Blob* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Blob_SerializeToString(Blob* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Blob_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          Blob *value = (Blob *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Blob_ParseFromString(Blob* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  Blob_ParseFromLongString(Blob* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  Blob_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = Blob_new(&BlobType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((Blob *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    Blob_getraw(Blob *self, void *closure)
    {
        
          if (! self->protobuf->has_raw()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert12(
                  self->protobuf->raw());

        
    }

    int
    Blob_setraw(Blob *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_raw();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The raw attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
        
          self->protobuf->set_raw(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Blob_getraw_size(Blob *self, void *closure)
    {
        
          if (! self->protobuf->has_raw_size()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->raw_size());

        
    }

    int
    Blob_setraw_size(Blob *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_raw_size();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The raw_size attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_raw_size(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Blob_getzlib_data(Blob *self, void *closure)
    {
        
          if (! self->protobuf->has_zlib_data()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert12(
                  self->protobuf->zlib_data());

        
    }

    int
    Blob_setzlib_data(Blob *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_zlib_data();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The zlib_data attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
        
          self->protobuf->set_zlib_data(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Blob_getlzma_data(Blob *self, void *closure)
    {
        
          if (! self->protobuf->has_lzma_data()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert12(
                  self->protobuf->lzma_data());

        
    }

    int
    Blob_setlzma_data(Blob *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lzma_data();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The lzma_data attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
        
          self->protobuf->set_lzma_data(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Blob_getOBSOLETE_bzip2_data(Blob *self, void *closure)
    {
        
          if (! self->protobuf->has_obsolete_bzip2_data()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert12(
                  self->protobuf->obsolete_bzip2_data());

        
    }

    int
    Blob_setOBSOLETE_bzip2_data(Blob *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_obsolete_bzip2_data();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The OBSOLETE_bzip2_data attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
        
          self->protobuf->set_obsolete_bzip2_data(protoValue);
        
      

      return 0;
    }
  

  int
  Blob_init(Blob *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *raw = NULL;
        
          PyObject *raw_size = NULL;
        
          PyObject *zlib_data = NULL;
        
          PyObject *lzma_data = NULL;
        
          PyObject *OBSOLETE_bzip2_data = NULL;
        

        static char *kwlist[] = {
          
            (char *) "raw",
          
            (char *) "raw_size",
          
            (char *) "zlib_data",
          
            (char *) "lzma_data",
          
            (char *) "OBSOLETE_bzip2_data",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &raw,&raw_size,&zlib_data,&lzma_data,&OBSOLETE_bzip2_data))
          return -1;

        
          if (raw) {
            if (Blob_setraw(self, raw, NULL) < 0) {
              return -1;
            }
          }
        
          if (raw_size) {
            if (Blob_setraw_size(self, raw_size, NULL) < 0) {
              return -1;
            }
          }
        
          if (zlib_data) {
            if (Blob_setzlib_data(self, zlib_data, NULL) < 0) {
              return -1;
            }
          }
        
          if (lzma_data) {
            if (Blob_setlzma_data(self, lzma_data, NULL) < 0) {
              return -1;
            }
          }
        
          if (OBSOLETE_bzip2_data) {
            if (Blob_setOBSOLETE_bzip2_data(self, OBSOLETE_bzip2_data, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  Blob_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &BlobType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          Blob *selfValue = (Blob *)self;
          Blob *otherValue = (Blob *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  Blob_repr(PyObject *selfObject)
  {
      Blob *self = (Blob *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "Blob(";

      
        
        result << "raw=";
        member = Blob_getraw(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "raw_size=";
        member = Blob_getraw_size(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "zlib_data=";
        member = Blob_getzlib_data(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lzma_data=";
        member = Blob_getlzma_data(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "OBSOLETE_bzip2_data=";
        member = Blob_getOBSOLETE_bzip2_data(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef Blob_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef Blob_getsetters[] = {
    
      {(char *)"raw",
       (getter)Blob_getraw, (setter)Blob_setraw,
       (char *)"",
       NULL},
    
      {(char *)"raw_size",
       (getter)Blob_getraw_size, (setter)Blob_setraw_size,
       (char *)"",
       NULL},
    
      {(char *)"zlib_data",
       (getter)Blob_getzlib_data, (setter)Blob_setzlib_data,
       (char *)"",
       NULL},
    
      {(char *)"lzma_data",
       (getter)Blob_getlzma_data, (setter)Blob_setlzma_data,
       (char *)"",
       NULL},
    
      {(char *)"OBSOLETE_bzip2_data",
       (getter)Blob_getOBSOLETE_bzip2_data, (setter)Blob_setOBSOLETE_bzip2_data,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef Blob_methods[] = {
      {"DebugString", (PyCFunction)Blob_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)Blob_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)Blob_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)Blob_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)Blob_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)Blob_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject BlobType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.Blob",  /*tp_name*/
      sizeof(Blob),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)Blob_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      Blob_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "Blob objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      Blob_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      Blob_methods,             /* tp_methods */
      Blob_members,             /* tp_members */
      Blob_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)Blob_init,      /* tp_init */
      0,                                      /* tp_alloc */
      Blob_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject BlobHeaderType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::BlobHeader *protobuf;
  } BlobHeader;

  void
  BlobHeader_dealloc(BlobHeader* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  BlobHeader_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      BlobHeader *self;

      self = (BlobHeader *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::BlobHeader();

      return (PyObject *)self;
  }

  PyObject *
  BlobHeader_DebugString(BlobHeader* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  BlobHeader_SerializeToString(BlobHeader* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  BlobHeader_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          BlobHeader *value = (BlobHeader *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  BlobHeader_ParseFromString(BlobHeader* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  BlobHeader_ParseFromLongString(BlobHeader* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  BlobHeader_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = BlobHeader_new(&BlobHeaderType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((BlobHeader *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    BlobHeader_gettype(BlobHeader *self, void *closure)
    {
        
          if (! self->protobuf->has_type()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert9(
                  self->protobuf->type());

        
    }

    int
    BlobHeader_settype(BlobHeader *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_type();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        bool reallocated = false;
        if (PyUnicode_Check(value)) {
          value = PyUnicode_AsEncodedString(value, "utf-8", NULL);
          reallocated = true;
        }

        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The type attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));
        if (reallocated) {
          Py_XDECREF(value);
        }

      

      
        
          self->protobuf->set_type(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    BlobHeader_getindexdata(BlobHeader *self, void *closure)
    {
        
          if (! self->protobuf->has_indexdata()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert12(
                  self->protobuf->indexdata());

        
    }

    int
    BlobHeader_setindexdata(BlobHeader *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_indexdata();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The indexdata attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
        
          self->protobuf->set_indexdata(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    BlobHeader_getdatasize(BlobHeader *self, void *closure)
    {
        
          if (! self->protobuf->has_datasize()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->datasize());

        
    }

    int
    BlobHeader_setdatasize(BlobHeader *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_datasize();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The datasize attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_datasize(protoValue);
        
      

      return 0;
    }
  

  int
  BlobHeader_init(BlobHeader *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *type = NULL;
        
          PyObject *indexdata = NULL;
        
          PyObject *datasize = NULL;
        

        static char *kwlist[] = {
          
            (char *) "type",
          
            (char *) "indexdata",
          
            (char *) "datasize",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOO", kwlist,
            &type,&indexdata,&datasize))
          return -1;

        
          if (type) {
            if (BlobHeader_settype(self, type, NULL) < 0) {
              return -1;
            }
          }
        
          if (indexdata) {
            if (BlobHeader_setindexdata(self, indexdata, NULL) < 0) {
              return -1;
            }
          }
        
          if (datasize) {
            if (BlobHeader_setdatasize(self, datasize, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  BlobHeader_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &BlobHeaderType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          BlobHeader *selfValue = (BlobHeader *)self;
          BlobHeader *otherValue = (BlobHeader *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  BlobHeader_repr(PyObject *selfObject)
  {
      BlobHeader *self = (BlobHeader *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "BlobHeader(";

      
        
        result << "type=";
        member = BlobHeader_gettype(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "indexdata=";
        member = BlobHeader_getindexdata(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "datasize=";
        member = BlobHeader_getdatasize(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef BlobHeader_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef BlobHeader_getsetters[] = {
    
      {(char *)"type",
       (getter)BlobHeader_gettype, (setter)BlobHeader_settype,
       (char *)"",
       NULL},
    
      {(char *)"indexdata",
       (getter)BlobHeader_getindexdata, (setter)BlobHeader_setindexdata,
       (char *)"",
       NULL},
    
      {(char *)"datasize",
       (getter)BlobHeader_getdatasize, (setter)BlobHeader_setdatasize,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef BlobHeader_methods[] = {
      {"DebugString", (PyCFunction)BlobHeader_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)BlobHeader_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)BlobHeader_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)BlobHeader_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)BlobHeader_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)BlobHeader_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject BlobHeaderType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.BlobHeader",  /*tp_name*/
      sizeof(BlobHeader),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)BlobHeader_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      BlobHeader_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "BlobHeader objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      BlobHeader_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      BlobHeader_methods,             /* tp_methods */
      BlobHeader_members,             /* tp_members */
      BlobHeader_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)BlobHeader_init,      /* tp_init */
      0,                                      /* tp_alloc */
      BlobHeader_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject ChangeSetType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::ChangeSet *protobuf;
  } ChangeSet;

  void
  ChangeSet_dealloc(ChangeSet* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  ChangeSet_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      ChangeSet *self;

      self = (ChangeSet *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::ChangeSet();

      return (PyObject *)self;
  }

  PyObject *
  ChangeSet_DebugString(ChangeSet* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  ChangeSet_SerializeToString(ChangeSet* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  ChangeSet_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          ChangeSet *value = (ChangeSet *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  ChangeSet_ParseFromString(ChangeSet* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  ChangeSet_ParseFromLongString(ChangeSet* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  ChangeSet_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = ChangeSet_new(&ChangeSetType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((ChangeSet *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    ChangeSet_getid(ChangeSet *self, void *closure)
    {
        
          if (! self->protobuf->has_id()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->id());

        
    }

    int
    ChangeSet_setid(ChangeSet *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_id();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The id attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_id(protoValue);
        
      

      return 0;
    }
  

  int
  ChangeSet_init(ChangeSet *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *id = NULL;
        

        static char *kwlist[] = {
          
            (char *) "id",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|O", kwlist,
            &id))
          return -1;

        
          if (id) {
            if (ChangeSet_setid(self, id, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  ChangeSet_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &ChangeSetType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          ChangeSet *selfValue = (ChangeSet *)self;
          ChangeSet *otherValue = (ChangeSet *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  ChangeSet_repr(PyObject *selfObject)
  {
      ChangeSet *self = (ChangeSet *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "ChangeSet(";

      
        
        result << "id=";
        member = ChangeSet_getid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef ChangeSet_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef ChangeSet_getsetters[] = {
    
      {(char *)"id",
       (getter)ChangeSet_getid, (setter)ChangeSet_setid,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef ChangeSet_methods[] = {
      {"DebugString", (PyCFunction)ChangeSet_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)ChangeSet_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)ChangeSet_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)ChangeSet_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)ChangeSet_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)ChangeSet_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject ChangeSetType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.ChangeSet",  /*tp_name*/
      sizeof(ChangeSet),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)ChangeSet_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      ChangeSet_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "ChangeSet objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      ChangeSet_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      ChangeSet_methods,             /* tp_methods */
      ChangeSet_members,             /* tp_members */
      ChangeSet_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)ChangeSet_init,      /* tp_init */
      0,                                      /* tp_alloc */
      ChangeSet_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject DenseInfoType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::DenseInfo *protobuf;
  } DenseInfo;

  void
  DenseInfo_dealloc(DenseInfo* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  DenseInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      DenseInfo *self;

      self = (DenseInfo *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::DenseInfo();

      return (PyObject *)self;
  }

  PyObject *
  DenseInfo_DebugString(DenseInfo* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseInfo_SerializeToString(DenseInfo* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseInfo_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          DenseInfo *value = (DenseInfo *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseInfo_ParseFromString(DenseInfo* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  DenseInfo_ParseFromLongString(DenseInfo* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  DenseInfo_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = DenseInfo_new(&DenseInfoType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((DenseInfo *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    DenseInfo_getversion(DenseInfo *self, void *closure)
    {
        
          int len = self->protobuf->version_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert5(
                    self->protobuf->version(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseInfo_setversion(DenseInfo *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_version();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The version attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The version attribute value must be a sequence");
        self->protobuf->clear_version();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The version attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_version(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseInfo_gettimestamp(DenseInfo *self, void *closure)
    {
        
          int len = self->protobuf->timestamp_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->timestamp(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseInfo_settimestamp(DenseInfo *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_timestamp();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The timestamp attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The timestamp attribute value must be a sequence");
        self->protobuf->clear_timestamp();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The timestamp attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_timestamp(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseInfo_getchangeset(DenseInfo *self, void *closure)
    {
        
          int len = self->protobuf->changeset_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->changeset(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseInfo_setchangeset(DenseInfo *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_changeset();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The changeset attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The changeset attribute value must be a sequence");
        self->protobuf->clear_changeset();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The changeset attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_changeset(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseInfo_getuid(DenseInfo *self, void *closure)
    {
        
          int len = self->protobuf->uid_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert17(
                    self->protobuf->uid(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseInfo_setuid(DenseInfo *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_uid();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The uid attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The uid attribute value must be a sequence");
        self->protobuf->clear_uid();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The uid attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_uid(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseInfo_getuser_sid(DenseInfo *self, void *closure)
    {
        
          int len = self->protobuf->user_sid_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert17(
                    self->protobuf->user_sid(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseInfo_setuser_sid(DenseInfo *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_user_sid();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The user_sid attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The user_sid attribute value must be a sequence");
        self->protobuf->clear_user_sid();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The user_sid attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_user_sid(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  DenseInfo_init(DenseInfo *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *version = NULL;
        
          PyObject *timestamp = NULL;
        
          PyObject *changeset = NULL;
        
          PyObject *uid = NULL;
        
          PyObject *user_sid = NULL;
        

        static char *kwlist[] = {
          
            (char *) "version",
          
            (char *) "timestamp",
          
            (char *) "changeset",
          
            (char *) "uid",
          
            (char *) "user_sid",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &version,&timestamp,&changeset,&uid,&user_sid))
          return -1;

        
          if (version) {
            if (DenseInfo_setversion(self, version, NULL) < 0) {
              return -1;
            }
          }
        
          if (timestamp) {
            if (DenseInfo_settimestamp(self, timestamp, NULL) < 0) {
              return -1;
            }
          }
        
          if (changeset) {
            if (DenseInfo_setchangeset(self, changeset, NULL) < 0) {
              return -1;
            }
          }
        
          if (uid) {
            if (DenseInfo_setuid(self, uid, NULL) < 0) {
              return -1;
            }
          }
        
          if (user_sid) {
            if (DenseInfo_setuser_sid(self, user_sid, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  DenseInfo_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &DenseInfoType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          DenseInfo *selfValue = (DenseInfo *)self;
          DenseInfo *otherValue = (DenseInfo *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  DenseInfo_repr(PyObject *selfObject)
  {
      DenseInfo *self = (DenseInfo *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "DenseInfo(";

      
        
        result << "version=";
        member = DenseInfo_getversion(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "timestamp=";
        member = DenseInfo_gettimestamp(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "changeset=";
        member = DenseInfo_getchangeset(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "uid=";
        member = DenseInfo_getuid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "user_sid=";
        member = DenseInfo_getuser_sid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef DenseInfo_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef DenseInfo_getsetters[] = {
    
      {(char *)"version",
       (getter)DenseInfo_getversion, (setter)DenseInfo_setversion,
       (char *)"",
       NULL},
    
      {(char *)"timestamp",
       (getter)DenseInfo_gettimestamp, (setter)DenseInfo_settimestamp,
       (char *)"",
       NULL},
    
      {(char *)"changeset",
       (getter)DenseInfo_getchangeset, (setter)DenseInfo_setchangeset,
       (char *)"",
       NULL},
    
      {(char *)"uid",
       (getter)DenseInfo_getuid, (setter)DenseInfo_setuid,
       (char *)"",
       NULL},
    
      {(char *)"user_sid",
       (getter)DenseInfo_getuser_sid, (setter)DenseInfo_setuser_sid,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef DenseInfo_methods[] = {
      {"DebugString", (PyCFunction)DenseInfo_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)DenseInfo_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)DenseInfo_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)DenseInfo_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)DenseInfo_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)DenseInfo_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject DenseInfoType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.DenseInfo",  /*tp_name*/
      sizeof(DenseInfo),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)DenseInfo_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      DenseInfo_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "DenseInfo objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      DenseInfo_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      DenseInfo_methods,             /* tp_methods */
      DenseInfo_members,             /* tp_members */
      DenseInfo_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)DenseInfo_init,      /* tp_init */
      0,                                      /* tp_alloc */
      DenseInfo_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject HeaderBBoxType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::HeaderBBox *protobuf;
  } HeaderBBox;

  void
  HeaderBBox_dealloc(HeaderBBox* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  HeaderBBox_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      HeaderBBox *self;

      self = (HeaderBBox *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::HeaderBBox();

      return (PyObject *)self;
  }

  PyObject *
  HeaderBBox_DebugString(HeaderBBox* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBBox_SerializeToString(HeaderBBox* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBBox_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          HeaderBBox *value = (HeaderBBox *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBBox_ParseFromString(HeaderBBox* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  HeaderBBox_ParseFromLongString(HeaderBBox* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  HeaderBBox_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = HeaderBBox_new(&HeaderBBoxType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((HeaderBBox *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    HeaderBBox_getleft(HeaderBBox *self, void *closure)
    {
        
          if (! self->protobuf->has_left()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->left());

        
    }

    int
    HeaderBBox_setleft(HeaderBBox *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_left();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The left attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_left(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    HeaderBBox_getright(HeaderBBox *self, void *closure)
    {
        
          if (! self->protobuf->has_right()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->right());

        
    }

    int
    HeaderBBox_setright(HeaderBBox *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_right();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The right attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_right(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    HeaderBBox_gettop(HeaderBBox *self, void *closure)
    {
        
          if (! self->protobuf->has_top()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->top());

        
    }

    int
    HeaderBBox_settop(HeaderBBox *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_top();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The top attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_top(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    HeaderBBox_getbottom(HeaderBBox *self, void *closure)
    {
        
          if (! self->protobuf->has_bottom()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->bottom());

        
    }

    int
    HeaderBBox_setbottom(HeaderBBox *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_bottom();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The bottom attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_bottom(protoValue);
        
      

      return 0;
    }
  

  int
  HeaderBBox_init(HeaderBBox *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *left = NULL;
        
          PyObject *right = NULL;
        
          PyObject *top = NULL;
        
          PyObject *bottom = NULL;
        

        static char *kwlist[] = {
          
            (char *) "left",
          
            (char *) "right",
          
            (char *) "top",
          
            (char *) "bottom",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOO", kwlist,
            &left,&right,&top,&bottom))
          return -1;

        
          if (left) {
            if (HeaderBBox_setleft(self, left, NULL) < 0) {
              return -1;
            }
          }
        
          if (right) {
            if (HeaderBBox_setright(self, right, NULL) < 0) {
              return -1;
            }
          }
        
          if (top) {
            if (HeaderBBox_settop(self, top, NULL) < 0) {
              return -1;
            }
          }
        
          if (bottom) {
            if (HeaderBBox_setbottom(self, bottom, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  HeaderBBox_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &HeaderBBoxType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          HeaderBBox *selfValue = (HeaderBBox *)self;
          HeaderBBox *otherValue = (HeaderBBox *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  HeaderBBox_repr(PyObject *selfObject)
  {
      HeaderBBox *self = (HeaderBBox *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "HeaderBBox(";

      
        
        result << "left=";
        member = HeaderBBox_getleft(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "right=";
        member = HeaderBBox_getright(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "top=";
        member = HeaderBBox_gettop(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "bottom=";
        member = HeaderBBox_getbottom(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef HeaderBBox_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef HeaderBBox_getsetters[] = {
    
      {(char *)"left",
       (getter)HeaderBBox_getleft, (setter)HeaderBBox_setleft,
       (char *)"",
       NULL},
    
      {(char *)"right",
       (getter)HeaderBBox_getright, (setter)HeaderBBox_setright,
       (char *)"",
       NULL},
    
      {(char *)"top",
       (getter)HeaderBBox_gettop, (setter)HeaderBBox_settop,
       (char *)"",
       NULL},
    
      {(char *)"bottom",
       (getter)HeaderBBox_getbottom, (setter)HeaderBBox_setbottom,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef HeaderBBox_methods[] = {
      {"DebugString", (PyCFunction)HeaderBBox_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)HeaderBBox_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)HeaderBBox_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)HeaderBBox_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)HeaderBBox_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)HeaderBBox_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject HeaderBBoxType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.HeaderBBox",  /*tp_name*/
      sizeof(HeaderBBox),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)HeaderBBox_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      HeaderBBox_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "HeaderBBox objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      HeaderBBox_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      HeaderBBox_methods,             /* tp_methods */
      HeaderBBox_members,             /* tp_members */
      HeaderBBox_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)HeaderBBox_init,      /* tp_init */
      0,                                      /* tp_alloc */
      HeaderBBox_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject InfoType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::Info *protobuf;
  } Info;

  void
  Info_dealloc(Info* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  Info_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      Info *self;

      self = (Info *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::Info();

      return (PyObject *)self;
  }

  PyObject *
  Info_DebugString(Info* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Info_SerializeToString(Info* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Info_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          Info *value = (Info *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Info_ParseFromString(Info* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  Info_ParseFromLongString(Info* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  Info_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = Info_new(&InfoType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((Info *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    Info_getversion(Info *self, void *closure)
    {
        
          if (! self->protobuf->has_version()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->version());

        
    }

    int
    Info_setversion(Info *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_version();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The version attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_version(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Info_gettimestamp(Info *self, void *closure)
    {
        
          if (! self->protobuf->has_timestamp()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->timestamp());

        
    }

    int
    Info_settimestamp(Info *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_timestamp();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The timestamp attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_timestamp(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Info_getchangeset(Info *self, void *closure)
    {
        
          if (! self->protobuf->has_changeset()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->changeset());

        
    }

    int
    Info_setchangeset(Info *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_changeset();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The changeset attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_changeset(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Info_getuid(Info *self, void *closure)
    {
        
          if (! self->protobuf->has_uid()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->uid());

        
    }

    int
    Info_setuid(Info *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_uid();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The uid attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_uid(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Info_getuser_sid(Info *self, void *closure)
    {
        
          if (! self->protobuf->has_user_sid()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert13(
                  self->protobuf->user_sid());

        
    }

    int
    Info_setuser_sid(Info *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_user_sid();
        return 0;
      }

      
        PyObject *value = input;
      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The user_sid attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_user_sid(protoValue);
        
      

      return 0;
    }
  

  int
  Info_init(Info *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *version = NULL;
        
          PyObject *timestamp = NULL;
        
          PyObject *changeset = NULL;
        
          PyObject *uid = NULL;
        
          PyObject *user_sid = NULL;
        

        static char *kwlist[] = {
          
            (char *) "version",
          
            (char *) "timestamp",
          
            (char *) "changeset",
          
            (char *) "uid",
          
            (char *) "user_sid",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &version,&timestamp,&changeset,&uid,&user_sid))
          return -1;

        
          if (version) {
            if (Info_setversion(self, version, NULL) < 0) {
              return -1;
            }
          }
        
          if (timestamp) {
            if (Info_settimestamp(self, timestamp, NULL) < 0) {
              return -1;
            }
          }
        
          if (changeset) {
            if (Info_setchangeset(self, changeset, NULL) < 0) {
              return -1;
            }
          }
        
          if (uid) {
            if (Info_setuid(self, uid, NULL) < 0) {
              return -1;
            }
          }
        
          if (user_sid) {
            if (Info_setuser_sid(self, user_sid, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  Info_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &InfoType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          Info *selfValue = (Info *)self;
          Info *otherValue = (Info *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  Info_repr(PyObject *selfObject)
  {
      Info *self = (Info *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "Info(";

      
        
        result << "version=";
        member = Info_getversion(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "timestamp=";
        member = Info_gettimestamp(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "changeset=";
        member = Info_getchangeset(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "uid=";
        member = Info_getuid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "user_sid=";
        member = Info_getuser_sid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef Info_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef Info_getsetters[] = {
    
      {(char *)"version",
       (getter)Info_getversion, (setter)Info_setversion,
       (char *)"",
       NULL},
    
      {(char *)"timestamp",
       (getter)Info_gettimestamp, (setter)Info_settimestamp,
       (char *)"",
       NULL},
    
      {(char *)"changeset",
       (getter)Info_getchangeset, (setter)Info_setchangeset,
       (char *)"",
       NULL},
    
      {(char *)"uid",
       (getter)Info_getuid, (setter)Info_setuid,
       (char *)"",
       NULL},
    
      {(char *)"user_sid",
       (getter)Info_getuser_sid, (setter)Info_setuser_sid,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef Info_methods[] = {
      {"DebugString", (PyCFunction)Info_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)Info_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)Info_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)Info_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)Info_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)Info_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject InfoType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.Info",  /*tp_name*/
      sizeof(Info),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)Info_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      Info_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "Info objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      Info_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      Info_methods,             /* tp_methods */
      Info_members,             /* tp_members */
      Info_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)Info_init,      /* tp_init */
      0,                                      /* tp_alloc */
      Info_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject StringTableType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::StringTable *protobuf;
  } StringTable;

  void
  StringTable_dealloc(StringTable* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  StringTable_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      StringTable *self;

      self = (StringTable *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::StringTable();

      return (PyObject *)self;
  }

  PyObject *
  StringTable_DebugString(StringTable* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  StringTable_SerializeToString(StringTable* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  StringTable_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          StringTable *value = (StringTable *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  StringTable_ParseFromString(StringTable* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  StringTable_ParseFromLongString(StringTable* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  StringTable_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = StringTable_new(&StringTableType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((StringTable *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    StringTable_gets(StringTable *self, void *closure)
    {
        
          int len = self->protobuf->s_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert12(
                    self->protobuf->s(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    StringTable_sets(StringTable *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_s();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The s attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The s attribute value must be a sequence");
        self->protobuf->clear_s();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        // string
        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The s attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));

      

      
          
            self->protobuf->add_s(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  StringTable_init(StringTable *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *s = NULL;
        

        static char *kwlist[] = {
          
            (char *) "s",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|O", kwlist,
            &s))
          return -1;

        
          if (s) {
            if (StringTable_sets(self, s, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  StringTable_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &StringTableType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          StringTable *selfValue = (StringTable *)self;
          StringTable *otherValue = (StringTable *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  StringTable_repr(PyObject *selfObject)
  {
      StringTable *self = (StringTable *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "StringTable(";

      
        
        result << "s=";
        member = StringTable_gets(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef StringTable_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef StringTable_getsetters[] = {
    
      {(char *)"s",
       (getter)StringTable_gets, (setter)StringTable_sets,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef StringTable_methods[] = {
      {"DebugString", (PyCFunction)StringTable_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)StringTable_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)StringTable_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)StringTable_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)StringTable_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)StringTable_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject StringTableType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.StringTable",  /*tp_name*/
      sizeof(StringTable),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)StringTable_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      StringTable_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "StringTable objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      StringTable_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      StringTable_methods,             /* tp_methods */
      StringTable_members,             /* tp_members */
      StringTable_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)StringTable_init,      /* tp_init */
      0,                                      /* tp_alloc */
      StringTable_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject DenseNodesType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::DenseNodes *protobuf;
  } DenseNodes;

  void
  DenseNodes_dealloc(DenseNodes* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  DenseNodes_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      DenseNodes *self;

      self = (DenseNodes *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::DenseNodes();

      return (PyObject *)self;
  }

  PyObject *
  DenseNodes_DebugString(DenseNodes* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseNodes_SerializeToString(DenseNodes* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseNodes_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          DenseNodes *value = (DenseNodes *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  DenseNodes_ParseFromString(DenseNodes* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  DenseNodes_ParseFromLongString(DenseNodes* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  DenseNodes_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = DenseNodes_new(&DenseNodesType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((DenseNodes *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    DenseNodes_getid(DenseNodes *self, void *closure)
    {
        
          int len = self->protobuf->id_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->id(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseNodes_setid(DenseNodes *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_id();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The id attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The id attribute value must be a sequence");
        self->protobuf->clear_id();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The id attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_id(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertDenseNodesdenseinfo(const ::google::protobuf::Message &value)
      {
          DenseInfo *obj = (DenseInfo *)
              DenseInfo_new(&DenseInfoType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    DenseNodes_getdenseinfo(DenseNodes *self, void *closure)
    {
        
          if (! self->protobuf->has_denseinfo()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertDenseNodesdenseinfo(
                  self->protobuf->denseinfo());

        
    }

    int
    DenseNodes_setdenseinfo(DenseNodes *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_denseinfo();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &DenseInfoType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The denseinfo attribute value must be an instance of DenseInfo");
          return -1;
        }

         // .OSMPBF.DenseInfo
        ::OSMPBF::DenseInfo *protoValue =
            ((DenseInfo *) value)->protobuf;

      

      
        
          self->protobuf->clear_denseinfo();
          self->protobuf->mutable_denseinfo()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    DenseNodes_getlat(DenseNodes *self, void *closure)
    {
        
          int len = self->protobuf->lat_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->lat(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseNodes_setlat(DenseNodes *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lat();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The lat attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The lat attribute value must be a sequence");
        self->protobuf->clear_lat();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lat attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_lat(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseNodes_getlon(DenseNodes *self, void *closure)
    {
        
          int len = self->protobuf->lon_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->lon(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseNodes_setlon(DenseNodes *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lon();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The lon attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The lon attribute value must be a sequence");
        self->protobuf->clear_lon();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lon attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_lon(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    DenseNodes_getkeys_vals(DenseNodes *self, void *closure)
    {
        
          int len = self->protobuf->keys_vals_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert5(
                    self->protobuf->keys_vals(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    DenseNodes_setkeys_vals(DenseNodes *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_keys_vals();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The keys_vals attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The keys_vals attribute value must be a sequence");
        self->protobuf->clear_keys_vals();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The keys_vals attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_keys_vals(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  DenseNodes_init(DenseNodes *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *id = NULL;
        
          PyObject *denseinfo = NULL;
        
          PyObject *lat = NULL;
        
          PyObject *lon = NULL;
        
          PyObject *keys_vals = NULL;
        

        static char *kwlist[] = {
          
            (char *) "id",
          
            (char *) "denseinfo",
          
            (char *) "lat",
          
            (char *) "lon",
          
            (char *) "keys_vals",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &id,&denseinfo,&lat,&lon,&keys_vals))
          return -1;

        
          if (id) {
            if (DenseNodes_setid(self, id, NULL) < 0) {
              return -1;
            }
          }
        
          if (denseinfo) {
            if (DenseNodes_setdenseinfo(self, denseinfo, NULL) < 0) {
              return -1;
            }
          }
        
          if (lat) {
            if (DenseNodes_setlat(self, lat, NULL) < 0) {
              return -1;
            }
          }
        
          if (lon) {
            if (DenseNodes_setlon(self, lon, NULL) < 0) {
              return -1;
            }
          }
        
          if (keys_vals) {
            if (DenseNodes_setkeys_vals(self, keys_vals, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  DenseNodes_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &DenseNodesType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          DenseNodes *selfValue = (DenseNodes *)self;
          DenseNodes *otherValue = (DenseNodes *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  DenseNodes_repr(PyObject *selfObject)
  {
      DenseNodes *self = (DenseNodes *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "DenseNodes(";

      
        
        result << "id=";
        member = DenseNodes_getid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "denseinfo=";
        member = DenseNodes_getdenseinfo(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lat=";
        member = DenseNodes_getlat(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lon=";
        member = DenseNodes_getlon(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "keys_vals=";
        member = DenseNodes_getkeys_vals(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef DenseNodes_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef DenseNodes_getsetters[] = {
    
      {(char *)"id",
       (getter)DenseNodes_getid, (setter)DenseNodes_setid,
       (char *)"",
       NULL},
    
      {(char *)"denseinfo",
       (getter)DenseNodes_getdenseinfo, (setter)DenseNodes_setdenseinfo,
       (char *)"",
       NULL},
    
      {(char *)"lat",
       (getter)DenseNodes_getlat, (setter)DenseNodes_setlat,
       (char *)"",
       NULL},
    
      {(char *)"lon",
       (getter)DenseNodes_getlon, (setter)DenseNodes_setlon,
       (char *)"",
       NULL},
    
      {(char *)"keys_vals",
       (getter)DenseNodes_getkeys_vals, (setter)DenseNodes_setkeys_vals,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef DenseNodes_methods[] = {
      {"DebugString", (PyCFunction)DenseNodes_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)DenseNodes_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)DenseNodes_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)DenseNodes_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)DenseNodes_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)DenseNodes_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject DenseNodesType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.DenseNodes",  /*tp_name*/
      sizeof(DenseNodes),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)DenseNodes_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      DenseNodes_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "DenseNodes objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      DenseNodes_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      DenseNodes_methods,             /* tp_methods */
      DenseNodes_members,             /* tp_members */
      DenseNodes_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)DenseNodes_init,      /* tp_init */
      0,                                      /* tp_alloc */
      DenseNodes_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject HeaderBlockType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::HeaderBlock *protobuf;
  } HeaderBlock;

  void
  HeaderBlock_dealloc(HeaderBlock* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  HeaderBlock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      HeaderBlock *self;

      self = (HeaderBlock *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::HeaderBlock();

      return (PyObject *)self;
  }

  PyObject *
  HeaderBlock_DebugString(HeaderBlock* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBlock_SerializeToString(HeaderBlock* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBlock_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          HeaderBlock *value = (HeaderBlock *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  HeaderBlock_ParseFromString(HeaderBlock* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  HeaderBlock_ParseFromLongString(HeaderBlock* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  HeaderBlock_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = HeaderBlock_new(&HeaderBlockType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((HeaderBlock *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    
      PyObject *
      fastpb_convertHeaderBlockbbox(const ::google::protobuf::Message &value)
      {
          HeaderBBox *obj = (HeaderBBox *)
              HeaderBBox_new(&HeaderBBoxType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    HeaderBlock_getbbox(HeaderBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_bbox()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertHeaderBlockbbox(
                  self->protobuf->bbox());

        
    }

    int
    HeaderBlock_setbbox(HeaderBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_bbox();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &HeaderBBoxType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The bbox attribute value must be an instance of HeaderBBox");
          return -1;
        }

         // .OSMPBF.HeaderBBox
        ::OSMPBF::HeaderBBox *protoValue =
            ((HeaderBBox *) value)->protobuf;

      

      
        
          self->protobuf->clear_bbox();
          self->protobuf->mutable_bbox()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    HeaderBlock_getrequired_features(HeaderBlock *self, void *closure)
    {
        
          int len = self->protobuf->required_features_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert9(
                    self->protobuf->required_features(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    HeaderBlock_setrequired_features(HeaderBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_required_features();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The required_features attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The required_features attribute value must be a sequence");
        self->protobuf->clear_required_features();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        // string
        bool reallocated = false;
        if (PyUnicode_Check(value)) {
          value = PyUnicode_AsEncodedString(value, "utf-8", NULL);
          reallocated = true;
        }

        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The required_features attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));
        if (reallocated) {
          Py_XDECREF(value);
        }

      

      
          
            self->protobuf->add_required_features(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    HeaderBlock_getoptional_features(HeaderBlock *self, void *closure)
    {
        
          int len = self->protobuf->optional_features_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert9(
                    self->protobuf->optional_features(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    HeaderBlock_setoptional_features(HeaderBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_optional_features();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The optional_features attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The optional_features attribute value must be a sequence");
        self->protobuf->clear_optional_features();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        // string
        bool reallocated = false;
        if (PyUnicode_Check(value)) {
          value = PyUnicode_AsEncodedString(value, "utf-8", NULL);
          reallocated = true;
        }

        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The optional_features attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));
        if (reallocated) {
          Py_XDECREF(value);
        }

      

      
          
            self->protobuf->add_optional_features(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    HeaderBlock_getwritingprogram(HeaderBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_writingprogram()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert9(
                  self->protobuf->writingprogram());

        
    }

    int
    HeaderBlock_setwritingprogram(HeaderBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_writingprogram();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        bool reallocated = false;
        if (PyUnicode_Check(value)) {
          value = PyUnicode_AsEncodedString(value, "utf-8", NULL);
          reallocated = true;
        }

        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The writingprogram attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));
        if (reallocated) {
          Py_XDECREF(value);
        }

      

      
        
          self->protobuf->set_writingprogram(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    HeaderBlock_getsource(HeaderBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_source()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert9(
                  self->protobuf->source());

        
    }

    int
    HeaderBlock_setsource(HeaderBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_source();
        return 0;
      }

      
        PyObject *value = input;
      

      
        // string
        bool reallocated = false;
        if (PyUnicode_Check(value)) {
          value = PyUnicode_AsEncodedString(value, "utf-8", NULL);
          reallocated = true;
        }

        if (! PyString_Check(value)) {
          PyErr_SetString(PyExc_TypeError, "The source attribute value must be a string");
          return -1;
        }

        std::string protoValue(PyString_AsString(value), PyString_Size(value));
        if (reallocated) {
          Py_XDECREF(value);
        }

      

      
        
          self->protobuf->set_source(protoValue);
        
      

      return 0;
    }
  

  int
  HeaderBlock_init(HeaderBlock *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *bbox = NULL;
        
          PyObject *required_features = NULL;
        
          PyObject *optional_features = NULL;
        
          PyObject *writingprogram = NULL;
        
          PyObject *source = NULL;
        

        static char *kwlist[] = {
          
            (char *) "bbox",
          
            (char *) "required_features",
          
            (char *) "optional_features",
          
            (char *) "writingprogram",
          
            (char *) "source",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &bbox,&required_features,&optional_features,&writingprogram,&source))
          return -1;

        
          if (bbox) {
            if (HeaderBlock_setbbox(self, bbox, NULL) < 0) {
              return -1;
            }
          }
        
          if (required_features) {
            if (HeaderBlock_setrequired_features(self, required_features, NULL) < 0) {
              return -1;
            }
          }
        
          if (optional_features) {
            if (HeaderBlock_setoptional_features(self, optional_features, NULL) < 0) {
              return -1;
            }
          }
        
          if (writingprogram) {
            if (HeaderBlock_setwritingprogram(self, writingprogram, NULL) < 0) {
              return -1;
            }
          }
        
          if (source) {
            if (HeaderBlock_setsource(self, source, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  HeaderBlock_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &HeaderBlockType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          HeaderBlock *selfValue = (HeaderBlock *)self;
          HeaderBlock *otherValue = (HeaderBlock *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  HeaderBlock_repr(PyObject *selfObject)
  {
      HeaderBlock *self = (HeaderBlock *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "HeaderBlock(";

      
        
        result << "bbox=";
        member = HeaderBlock_getbbox(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "required_features=";
        member = HeaderBlock_getrequired_features(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "optional_features=";
        member = HeaderBlock_getoptional_features(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "writingprogram=";
        member = HeaderBlock_getwritingprogram(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "source=";
        member = HeaderBlock_getsource(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef HeaderBlock_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef HeaderBlock_getsetters[] = {
    
      {(char *)"bbox",
       (getter)HeaderBlock_getbbox, (setter)HeaderBlock_setbbox,
       (char *)"",
       NULL},
    
      {(char *)"required_features",
       (getter)HeaderBlock_getrequired_features, (setter)HeaderBlock_setrequired_features,
       (char *)"",
       NULL},
    
      {(char *)"optional_features",
       (getter)HeaderBlock_getoptional_features, (setter)HeaderBlock_setoptional_features,
       (char *)"",
       NULL},
    
      {(char *)"writingprogram",
       (getter)HeaderBlock_getwritingprogram, (setter)HeaderBlock_setwritingprogram,
       (char *)"",
       NULL},
    
      {(char *)"source",
       (getter)HeaderBlock_getsource, (setter)HeaderBlock_setsource,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef HeaderBlock_methods[] = {
      {"DebugString", (PyCFunction)HeaderBlock_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)HeaderBlock_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)HeaderBlock_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)HeaderBlock_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)HeaderBlock_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)HeaderBlock_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject HeaderBlockType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.HeaderBlock",  /*tp_name*/
      sizeof(HeaderBlock),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)HeaderBlock_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      HeaderBlock_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "HeaderBlock objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      HeaderBlock_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      HeaderBlock_methods,             /* tp_methods */
      HeaderBlock_members,             /* tp_members */
      HeaderBlock_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)HeaderBlock_init,      /* tp_init */
      0,                                      /* tp_alloc */
      HeaderBlock_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject NodeType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::Node *protobuf;
  } Node;

  void
  Node_dealloc(Node* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  Node_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      Node *self;

      self = (Node *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::Node();

      return (PyObject *)self;
  }

  PyObject *
  Node_DebugString(Node* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Node_SerializeToString(Node* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Node_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          Node *value = (Node *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Node_ParseFromString(Node* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  Node_ParseFromLongString(Node* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  Node_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = Node_new(&NodeType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((Node *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    Node_getid(Node *self, void *closure)
    {
        
          if (! self->protobuf->has_id()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->id());

        
    }

    int
    Node_setid(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_id();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The id attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_id(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Node_getkeys(Node *self, void *closure)
    {
        
          int len = self->protobuf->keys_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->keys(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Node_setkeys(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_keys();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The keys attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The keys attribute value must be a sequence");
        self->protobuf->clear_keys();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The keys attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_keys(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    Node_getvals(Node *self, void *closure)
    {
        
          int len = self->protobuf->vals_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->vals(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Node_setvals(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_vals();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The vals attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The vals attribute value must be a sequence");
        self->protobuf->clear_vals();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The vals attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_vals(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertNodeinfo(const ::google::protobuf::Message &value)
      {
          Info *obj = (Info *)
              Info_new(&InfoType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    Node_getinfo(Node *self, void *closure)
    {
        
          if (! self->protobuf->has_info()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertNodeinfo(
                  self->protobuf->info());

        
    }

    int
    Node_setinfo(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_info();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &InfoType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The info attribute value must be an instance of Info");
          return -1;
        }

         // .OSMPBF.Info
        ::OSMPBF::Info *protoValue =
            ((Info *) value)->protobuf;

      

      
        
          self->protobuf->clear_info();
          self->protobuf->mutable_info()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Node_getlat(Node *self, void *closure)
    {
        
          if (! self->protobuf->has_lat()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->lat());

        
    }

    int
    Node_setlat(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lat();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lat attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_lat(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Node_getlon(Node *self, void *closure)
    {
        
          if (! self->protobuf->has_lon()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert18(
                  self->protobuf->lon());

        
    }

    int
    Node_setlon(Node *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lon();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lon attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_lon(protoValue);
        
      

      return 0;
    }
  

  int
  Node_init(Node *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *id = NULL;
        
          PyObject *keys = NULL;
        
          PyObject *vals = NULL;
        
          PyObject *info = NULL;
        
          PyObject *lat = NULL;
        
          PyObject *lon = NULL;
        

        static char *kwlist[] = {
          
            (char *) "id",
          
            (char *) "keys",
          
            (char *) "vals",
          
            (char *) "info",
          
            (char *) "lat",
          
            (char *) "lon",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOOO", kwlist,
            &id,&keys,&vals,&info,&lat,&lon))
          return -1;

        
          if (id) {
            if (Node_setid(self, id, NULL) < 0) {
              return -1;
            }
          }
        
          if (keys) {
            if (Node_setkeys(self, keys, NULL) < 0) {
              return -1;
            }
          }
        
          if (vals) {
            if (Node_setvals(self, vals, NULL) < 0) {
              return -1;
            }
          }
        
          if (info) {
            if (Node_setinfo(self, info, NULL) < 0) {
              return -1;
            }
          }
        
          if (lat) {
            if (Node_setlat(self, lat, NULL) < 0) {
              return -1;
            }
          }
        
          if (lon) {
            if (Node_setlon(self, lon, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  Node_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &NodeType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          Node *selfValue = (Node *)self;
          Node *otherValue = (Node *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  Node_repr(PyObject *selfObject)
  {
      Node *self = (Node *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "Node(";

      
        
        result << "id=";
        member = Node_getid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "keys=";
        member = Node_getkeys(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "vals=";
        member = Node_getvals(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "info=";
        member = Node_getinfo(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lat=";
        member = Node_getlat(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lon=";
        member = Node_getlon(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef Node_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef Node_getsetters[] = {
    
      {(char *)"id",
       (getter)Node_getid, (setter)Node_setid,
       (char *)"",
       NULL},
    
      {(char *)"keys",
       (getter)Node_getkeys, (setter)Node_setkeys,
       (char *)"",
       NULL},
    
      {(char *)"vals",
       (getter)Node_getvals, (setter)Node_setvals,
       (char *)"",
       NULL},
    
      {(char *)"info",
       (getter)Node_getinfo, (setter)Node_setinfo,
       (char *)"",
       NULL},
    
      {(char *)"lat",
       (getter)Node_getlat, (setter)Node_setlat,
       (char *)"",
       NULL},
    
      {(char *)"lon",
       (getter)Node_getlon, (setter)Node_setlon,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef Node_methods[] = {
      {"DebugString", (PyCFunction)Node_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)Node_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)Node_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)Node_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)Node_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)Node_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject NodeType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.Node",  /*tp_name*/
      sizeof(Node),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)Node_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      Node_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "Node objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      Node_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      Node_methods,             /* tp_methods */
      Node_members,             /* tp_members */
      Node_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)Node_init,      /* tp_init */
      0,                                      /* tp_alloc */
      Node_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject RelationType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::Relation *protobuf;
  } Relation;

  void
  Relation_dealloc(Relation* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  Relation_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      Relation *self;

      self = (Relation *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::Relation();

      return (PyObject *)self;
  }

  PyObject *
  Relation_DebugString(Relation* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Relation_SerializeToString(Relation* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Relation_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          Relation *value = (Relation *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Relation_ParseFromString(Relation* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  Relation_ParseFromLongString(Relation* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  Relation_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = Relation_new(&RelationType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((Relation *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    Relation_getid(Relation *self, void *closure)
    {
        
          if (! self->protobuf->has_id()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->id());

        
    }

    int
    Relation_setid(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_id();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The id attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_id(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Relation_getkeys(Relation *self, void *closure)
    {
        
          int len = self->protobuf->keys_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->keys(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Relation_setkeys(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_keys();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The keys attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The keys attribute value must be a sequence");
        self->protobuf->clear_keys();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The keys attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_keys(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    Relation_getvals(Relation *self, void *closure)
    {
        
          int len = self->protobuf->vals_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->vals(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Relation_setvals(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_vals();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The vals attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The vals attribute value must be a sequence");
        self->protobuf->clear_vals();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The vals attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_vals(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertRelationinfo(const ::google::protobuf::Message &value)
      {
          Info *obj = (Info *)
              Info_new(&InfoType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    Relation_getinfo(Relation *self, void *closure)
    {
        
          if (! self->protobuf->has_info()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertRelationinfo(
                  self->protobuf->info());

        
    }

    int
    Relation_setinfo(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_info();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &InfoType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The info attribute value must be an instance of Info");
          return -1;
        }

         // .OSMPBF.Info
        ::OSMPBF::Info *protoValue =
            ((Info *) value)->protobuf;

      

      
        
          self->protobuf->clear_info();
          self->protobuf->mutable_info()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Relation_getroles_sid(Relation *self, void *closure)
    {
        
          int len = self->protobuf->roles_sid_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert5(
                    self->protobuf->roles_sid(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Relation_setroles_sid(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_roles_sid();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The roles_sid attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The roles_sid attribute value must be a sequence");
        self->protobuf->clear_roles_sid();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The roles_sid attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_roles_sid(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    Relation_getmemids(Relation *self, void *closure)
    {
        
          int len = self->protobuf->memids_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->memids(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Relation_setmemids(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_memids();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The memids attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The memids attribute value must be a sequence");
        self->protobuf->clear_memids();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The memids attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_memids(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    Relation_gettypes(Relation *self, void *closure)
    {
        
          int len = self->protobuf->types_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert14(
                    self->protobuf->types(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Relation_settypes(Relation *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_types();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The types attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The types attribute value must be a sequence");
        self->protobuf->clear_types();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        // .OSMPBF.Relation.MemberType
        ::OSMPBF::Relation::MemberType protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = (::OSMPBF::Relation::MemberType) PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The types attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_types(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  Relation_init(Relation *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *id = NULL;
        
          PyObject *keys = NULL;
        
          PyObject *vals = NULL;
        
          PyObject *info = NULL;
        
          PyObject *roles_sid = NULL;
        
          PyObject *memids = NULL;
        
          PyObject *types = NULL;
        

        static char *kwlist[] = {
          
            (char *) "id",
          
            (char *) "keys",
          
            (char *) "vals",
          
            (char *) "info",
          
            (char *) "roles_sid",
          
            (char *) "memids",
          
            (char *) "types",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOOOO", kwlist,
            &id,&keys,&vals,&info,&roles_sid,&memids,&types))
          return -1;

        
          if (id) {
            if (Relation_setid(self, id, NULL) < 0) {
              return -1;
            }
          }
        
          if (keys) {
            if (Relation_setkeys(self, keys, NULL) < 0) {
              return -1;
            }
          }
        
          if (vals) {
            if (Relation_setvals(self, vals, NULL) < 0) {
              return -1;
            }
          }
        
          if (info) {
            if (Relation_setinfo(self, info, NULL) < 0) {
              return -1;
            }
          }
        
          if (roles_sid) {
            if (Relation_setroles_sid(self, roles_sid, NULL) < 0) {
              return -1;
            }
          }
        
          if (memids) {
            if (Relation_setmemids(self, memids, NULL) < 0) {
              return -1;
            }
          }
        
          if (types) {
            if (Relation_settypes(self, types, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  Relation_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &RelationType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          Relation *selfValue = (Relation *)self;
          Relation *otherValue = (Relation *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  Relation_repr(PyObject *selfObject)
  {
      Relation *self = (Relation *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "Relation(";

      
        
        result << "id=";
        member = Relation_getid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "keys=";
        member = Relation_getkeys(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "vals=";
        member = Relation_getvals(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "info=";
        member = Relation_getinfo(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "roles_sid=";
        member = Relation_getroles_sid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "memids=";
        member = Relation_getmemids(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "types=";
        member = Relation_gettypes(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef Relation_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef Relation_getsetters[] = {
    
      {(char *)"id",
       (getter)Relation_getid, (setter)Relation_setid,
       (char *)"",
       NULL},
    
      {(char *)"keys",
       (getter)Relation_getkeys, (setter)Relation_setkeys,
       (char *)"",
       NULL},
    
      {(char *)"vals",
       (getter)Relation_getvals, (setter)Relation_setvals,
       (char *)"",
       NULL},
    
      {(char *)"info",
       (getter)Relation_getinfo, (setter)Relation_setinfo,
       (char *)"",
       NULL},
    
      {(char *)"roles_sid",
       (getter)Relation_getroles_sid, (setter)Relation_setroles_sid,
       (char *)"",
       NULL},
    
      {(char *)"memids",
       (getter)Relation_getmemids, (setter)Relation_setmemids,
       (char *)"",
       NULL},
    
      {(char *)"types",
       (getter)Relation_gettypes, (setter)Relation_settypes,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef Relation_methods[] = {
      {"DebugString", (PyCFunction)Relation_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)Relation_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)Relation_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)Relation_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)Relation_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)Relation_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject RelationType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.Relation",  /*tp_name*/
      sizeof(Relation),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)Relation_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      Relation_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "Relation objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      Relation_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      Relation_methods,             /* tp_methods */
      Relation_members,             /* tp_members */
      Relation_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)Relation_init,      /* tp_init */
      0,                                      /* tp_alloc */
      Relation_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject WayType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::Way *protobuf;
  } Way;

  void
  Way_dealloc(Way* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  Way_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      Way *self;

      self = (Way *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::Way();

      return (PyObject *)self;
  }

  PyObject *
  Way_DebugString(Way* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Way_SerializeToString(Way* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Way_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          Way *value = (Way *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  Way_ParseFromString(Way* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  Way_ParseFromLongString(Way* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  Way_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = Way_new(&WayType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((Way *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    

    PyObject *
    Way_getid(Way *self, void *closure)
    {
        
          if (! self->protobuf->has_id()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->id());

        
    }

    int
    Way_setid(Way *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_id();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The id attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_id(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Way_getkeys(Way *self, void *closure)
    {
        
          int len = self->protobuf->keys_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->keys(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Way_setkeys(Way *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_keys();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The keys attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The keys attribute value must be a sequence");
        self->protobuf->clear_keys();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The keys attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_keys(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    Way_getvals(Way *self, void *closure)
    {
        
          int len = self->protobuf->vals_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert13(
                    self->protobuf->vals(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Way_setvals(Way *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_vals();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The vals attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The vals attribute value must be a sequence");
        self->protobuf->clear_vals();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        
          ::google::protobuf::uint32 protoValue;
        

        // uint32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsUnsignedLongMask(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsUnsignedLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The vals attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_vals(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertWayinfo(const ::google::protobuf::Message &value)
      {
          Info *obj = (Info *)
              Info_new(&InfoType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    Way_getinfo(Way *self, void *closure)
    {
        
          if (! self->protobuf->has_info()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertWayinfo(
                  self->protobuf->info());

        
    }

    int
    Way_setinfo(Way *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_info();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &InfoType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The info attribute value must be an instance of Info");
          return -1;
        }

         // .OSMPBF.Info
        ::OSMPBF::Info *protoValue =
            ((Info *) value)->protobuf;

      

      
        
          self->protobuf->clear_info();
          self->protobuf->mutable_info()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    Way_getrefs(Way *self, void *closure)
    {
        
          int len = self->protobuf->refs_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convert18(
                    self->protobuf->refs(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    Way_setrefs(Way *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_refs();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The refs attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The refs attribute value must be a sequence");
        self->protobuf->clear_refs();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The refs attribute value must be an integer");
          return -1;
        }

      

      
          
            self->protobuf->add_refs(protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  Way_init(Way *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *id = NULL;
        
          PyObject *keys = NULL;
        
          PyObject *vals = NULL;
        
          PyObject *info = NULL;
        
          PyObject *refs = NULL;
        

        static char *kwlist[] = {
          
            (char *) "id",
          
            (char *) "keys",
          
            (char *) "vals",
          
            (char *) "info",
          
            (char *) "refs",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &id,&keys,&vals,&info,&refs))
          return -1;

        
          if (id) {
            if (Way_setid(self, id, NULL) < 0) {
              return -1;
            }
          }
        
          if (keys) {
            if (Way_setkeys(self, keys, NULL) < 0) {
              return -1;
            }
          }
        
          if (vals) {
            if (Way_setvals(self, vals, NULL) < 0) {
              return -1;
            }
          }
        
          if (info) {
            if (Way_setinfo(self, info, NULL) < 0) {
              return -1;
            }
          }
        
          if (refs) {
            if (Way_setrefs(self, refs, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  Way_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &WayType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          Way *selfValue = (Way *)self;
          Way *otherValue = (Way *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  Way_repr(PyObject *selfObject)
  {
      Way *self = (Way *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "Way(";

      
        
        result << "id=";
        member = Way_getid(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "keys=";
        member = Way_getkeys(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "vals=";
        member = Way_getvals(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "info=";
        member = Way_getinfo(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "refs=";
        member = Way_getrefs(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef Way_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef Way_getsetters[] = {
    
      {(char *)"id",
       (getter)Way_getid, (setter)Way_setid,
       (char *)"",
       NULL},
    
      {(char *)"keys",
       (getter)Way_getkeys, (setter)Way_setkeys,
       (char *)"",
       NULL},
    
      {(char *)"vals",
       (getter)Way_getvals, (setter)Way_setvals,
       (char *)"",
       NULL},
    
      {(char *)"info",
       (getter)Way_getinfo, (setter)Way_setinfo,
       (char *)"",
       NULL},
    
      {(char *)"refs",
       (getter)Way_getrefs, (setter)Way_setrefs,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef Way_methods[] = {
      {"DebugString", (PyCFunction)Way_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)Way_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)Way_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)Way_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)Way_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)Way_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject WayType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.Way",  /*tp_name*/
      sizeof(Way),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)Way_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      Way_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "Way objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      Way_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      Way_methods,             /* tp_methods */
      Way_members,             /* tp_members */
      Way_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)Way_init,      /* tp_init */
      0,                                      /* tp_alloc */
      Way_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject PrimitiveGroupType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::PrimitiveGroup *protobuf;
  } PrimitiveGroup;

  void
  PrimitiveGroup_dealloc(PrimitiveGroup* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  PrimitiveGroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      PrimitiveGroup *self;

      self = (PrimitiveGroup *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::PrimitiveGroup();

      return (PyObject *)self;
  }

  PyObject *
  PrimitiveGroup_DebugString(PrimitiveGroup* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveGroup_SerializeToString(PrimitiveGroup* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveGroup_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PrimitiveGroup *value = (PrimitiveGroup *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveGroup_ParseFromString(PrimitiveGroup* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  PrimitiveGroup_ParseFromLongString(PrimitiveGroup* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  PrimitiveGroup_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = PrimitiveGroup_new(&PrimitiveGroupType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((PrimitiveGroup *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    
      PyObject *
      fastpb_convertPrimitiveGroupnodes(const ::google::protobuf::Message &value)
      {
          Node *obj = (Node *)
              Node_new(&NodeType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveGroup_getnodes(PrimitiveGroup *self, void *closure)
    {
        
          int len = self->protobuf->nodes_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convertPrimitiveGroupnodes(
                    self->protobuf->nodes(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    PrimitiveGroup_setnodes(PrimitiveGroup *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_nodes();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The nodes attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The nodes attribute value must be a sequence");
        self->protobuf->clear_nodes();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      

        if (!PyType_IsSubtype(value->ob_type, &NodeType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The nodes attribute value must be an instance of Node");
          return -1;
        }

         // .OSMPBF.Node
        ::OSMPBF::Node *protoValue =
            ((Node *) value)->protobuf;

      

      
          
            self->protobuf->add_nodes()->MergeFrom(*protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertPrimitiveGroupdense(const ::google::protobuf::Message &value)
      {
          DenseNodes *obj = (DenseNodes *)
              DenseNodes_new(&DenseNodesType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveGroup_getdense(PrimitiveGroup *self, void *closure)
    {
        
          if (! self->protobuf->has_dense()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertPrimitiveGroupdense(
                  self->protobuf->dense());

        
    }

    int
    PrimitiveGroup_setdense(PrimitiveGroup *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_dense();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &DenseNodesType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The dense attribute value must be an instance of DenseNodes");
          return -1;
        }

         // .OSMPBF.DenseNodes
        ::OSMPBF::DenseNodes *protoValue =
            ((DenseNodes *) value)->protobuf;

      

      
        
          self->protobuf->clear_dense();
          self->protobuf->mutable_dense()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertPrimitiveGroupways(const ::google::protobuf::Message &value)
      {
          Way *obj = (Way *)
              Way_new(&WayType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveGroup_getways(PrimitiveGroup *self, void *closure)
    {
        
          int len = self->protobuf->ways_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convertPrimitiveGroupways(
                    self->protobuf->ways(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    PrimitiveGroup_setways(PrimitiveGroup *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_ways();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The ways attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The ways attribute value must be a sequence");
        self->protobuf->clear_ways();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      

        if (!PyType_IsSubtype(value->ob_type, &WayType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The ways attribute value must be an instance of Way");
          return -1;
        }

         // .OSMPBF.Way
        ::OSMPBF::Way *protoValue =
            ((Way *) value)->protobuf;

      

      
          
            self->protobuf->add_ways()->MergeFrom(*protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertPrimitiveGrouprelations(const ::google::protobuf::Message &value)
      {
          Relation *obj = (Relation *)
              Relation_new(&RelationType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveGroup_getrelations(PrimitiveGroup *self, void *closure)
    {
        
          int len = self->protobuf->relations_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convertPrimitiveGrouprelations(
                    self->protobuf->relations(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    PrimitiveGroup_setrelations(PrimitiveGroup *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_relations();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The relations attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The relations attribute value must be a sequence");
        self->protobuf->clear_relations();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      

        if (!PyType_IsSubtype(value->ob_type, &RelationType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The relations attribute value must be an instance of Relation");
          return -1;
        }

         // .OSMPBF.Relation
        ::OSMPBF::Relation *protoValue =
            ((Relation *) value)->protobuf;

      

      
          
            self->protobuf->add_relations()->MergeFrom(*protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertPrimitiveGroupchangesets(const ::google::protobuf::Message &value)
      {
          ChangeSet *obj = (ChangeSet *)
              ChangeSet_new(&ChangeSetType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveGroup_getchangesets(PrimitiveGroup *self, void *closure)
    {
        
          int len = self->protobuf->changesets_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convertPrimitiveGroupchangesets(
                    self->protobuf->changesets(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    PrimitiveGroup_setchangesets(PrimitiveGroup *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_changesets();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The changesets attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The changesets attribute value must be a sequence");
        self->protobuf->clear_changesets();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      

        if (!PyType_IsSubtype(value->ob_type, &ChangeSetType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The changesets attribute value must be an instance of ChangeSet");
          return -1;
        }

         // .OSMPBF.ChangeSet
        ::OSMPBF::ChangeSet *protoValue =
            ((ChangeSet *) value)->protobuf;

      

      
          
            self->protobuf->add_changesets()->MergeFrom(*protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  

  int
  PrimitiveGroup_init(PrimitiveGroup *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *nodes = NULL;
        
          PyObject *dense = NULL;
        
          PyObject *ways = NULL;
        
          PyObject *relations = NULL;
        
          PyObject *changesets = NULL;
        

        static char *kwlist[] = {
          
            (char *) "nodes",
          
            (char *) "dense",
          
            (char *) "ways",
          
            (char *) "relations",
          
            (char *) "changesets",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOO", kwlist,
            &nodes,&dense,&ways,&relations,&changesets))
          return -1;

        
          if (nodes) {
            if (PrimitiveGroup_setnodes(self, nodes, NULL) < 0) {
              return -1;
            }
          }
        
          if (dense) {
            if (PrimitiveGroup_setdense(self, dense, NULL) < 0) {
              return -1;
            }
          }
        
          if (ways) {
            if (PrimitiveGroup_setways(self, ways, NULL) < 0) {
              return -1;
            }
          }
        
          if (relations) {
            if (PrimitiveGroup_setrelations(self, relations, NULL) < 0) {
              return -1;
            }
          }
        
          if (changesets) {
            if (PrimitiveGroup_setchangesets(self, changesets, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  PrimitiveGroup_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &PrimitiveGroupType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          PrimitiveGroup *selfValue = (PrimitiveGroup *)self;
          PrimitiveGroup *otherValue = (PrimitiveGroup *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  PrimitiveGroup_repr(PyObject *selfObject)
  {
      PrimitiveGroup *self = (PrimitiveGroup *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "PrimitiveGroup(";

      
        
        result << "nodes=";
        member = PrimitiveGroup_getnodes(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "dense=";
        member = PrimitiveGroup_getdense(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "ways=";
        member = PrimitiveGroup_getways(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "relations=";
        member = PrimitiveGroup_getrelations(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "changesets=";
        member = PrimitiveGroup_getchangesets(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef PrimitiveGroup_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef PrimitiveGroup_getsetters[] = {
    
      {(char *)"nodes",
       (getter)PrimitiveGroup_getnodes, (setter)PrimitiveGroup_setnodes,
       (char *)"",
       NULL},
    
      {(char *)"dense",
       (getter)PrimitiveGroup_getdense, (setter)PrimitiveGroup_setdense,
       (char *)"",
       NULL},
    
      {(char *)"ways",
       (getter)PrimitiveGroup_getways, (setter)PrimitiveGroup_setways,
       (char *)"",
       NULL},
    
      {(char *)"relations",
       (getter)PrimitiveGroup_getrelations, (setter)PrimitiveGroup_setrelations,
       (char *)"",
       NULL},
    
      {(char *)"changesets",
       (getter)PrimitiveGroup_getchangesets, (setter)PrimitiveGroup_setchangesets,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef PrimitiveGroup_methods[] = {
      {"DebugString", (PyCFunction)PrimitiveGroup_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)PrimitiveGroup_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)PrimitiveGroup_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)PrimitiveGroup_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)PrimitiveGroup_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)PrimitiveGroup_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject PrimitiveGroupType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.PrimitiveGroup",  /*tp_name*/
      sizeof(PrimitiveGroup),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)PrimitiveGroup_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      PrimitiveGroup_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "PrimitiveGroup objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      PrimitiveGroup_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      PrimitiveGroup_methods,             /* tp_methods */
      PrimitiveGroup_members,             /* tp_members */
      PrimitiveGroup_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)PrimitiveGroup_init,      /* tp_init */
      0,                                      /* tp_alloc */
      PrimitiveGroup_new,                 /* tp_new */
  };
}



// Lets try not to pollute the global namespace
namespace {

  // Forward-declaration for recursive structures
  extern PyTypeObject PrimitiveBlockType;

  typedef struct {
      PyObject_HEAD

      OSMPBF::PrimitiveBlock *protobuf;
  } PrimitiveBlock;

  void
  PrimitiveBlock_dealloc(PrimitiveBlock* self)
  {
      delete self->protobuf;
      self->ob_type->tp_free((PyObject*)self);
  }

  PyObject *
  PrimitiveBlock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
      PrimitiveBlock *self;

      self = (PrimitiveBlock *)type->tp_alloc(type, 0);

      self->protobuf = new OSMPBF::PrimitiveBlock();

      return (PyObject *)self;
  }

  PyObject *
  PrimitiveBlock_DebugString(PrimitiveBlock* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      result = self->protobuf->Utf8DebugString();
      Py_END_ALLOW_THREADS
      return PyUnicode_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveBlock_SerializeToString(PrimitiveBlock* self)
  {
      std::string result;
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->SerializeToString(&result);
      Py_END_ALLOW_THREADS
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveBlock_SerializeMany(void *nothing, PyObject *values)
  {
      std::string result;
      google::protobuf::io::ZeroCopyOutputStream* output =
          new google::protobuf::io::StringOutputStream(&result);
      google::protobuf::io::CodedOutputStream* outputStream =
          new google::protobuf::io::CodedOutputStream(output);

      PyObject *sequence = PySequence_Fast(values, "The values to serialize must be a sequence.");
      for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PrimitiveBlock *value = (PrimitiveBlock *)PySequence_Fast_GET_ITEM(sequence, i);

          Py_BEGIN_ALLOW_THREADS
          outputStream->WriteVarint32(value->protobuf->ByteSize());
          value->protobuf->SerializeToCodedStream(outputStream);
          Py_END_ALLOW_THREADS
      }

      Py_XDECREF(sequence);
      delete outputStream;
      delete output;
      return PyString_FromStringAndSize(result.data(), result.length());
  }


  PyObject *
  PrimitiveBlock_ParseFromString(PrimitiveBlock* self, PyObject *value)
  {
      std::string serialized(PyString_AsString(value), PyString_Size(value));
      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromString(serialized);
      Py_END_ALLOW_THREADS
      Py_RETURN_NONE;
  }


  PyObject *
  PrimitiveBlock_ParseFromLongString(PrimitiveBlock* self, PyObject *value)
  {
      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      Py_BEGIN_ALLOW_THREADS
      self->protobuf->ParseFromCodedStream(inputStream);
      Py_END_ALLOW_THREADS

      delete inputStream;
      delete input;

      Py_RETURN_NONE;
  }


  PyObject *
  PrimitiveBlock_ParseMany(void* nothing, PyObject *args)
  {
      PyObject *value;
      PyObject *callback;
      int fail = 0;

      if (!PyArg_ParseTuple(args, "OO", &value, &callback)) {
          return NULL;
      }

      google::protobuf::io::ZeroCopyInputStream* input =
          new google::protobuf::io::ArrayInputStream(PyString_AsString(value), PyString_Size(value));
      google::protobuf::io::CodedInputStream* inputStream =
          new google::protobuf::io::CodedInputStream(input);
      inputStream->SetTotalBytesLimit(512 * 1024 * 1024, 512 * 1024 * 1024);

      google::protobuf::uint32 bytes;
      PyObject *single = NULL;
      while (inputStream->ReadVarint32(&bytes)) {
          google::protobuf::io::CodedInputStream::Limit messageLimit = inputStream->PushLimit(bytes);

          if (single == NULL) {
            single = PrimitiveBlock_new(&PrimitiveBlockType, NULL, NULL);
          }

          Py_BEGIN_ALLOW_THREADS
          ((PrimitiveBlock *)single)->protobuf->ParseFromCodedStream(inputStream);
          Py_END_ALLOW_THREADS

          inputStream->PopLimit(messageLimit);
          PyObject *result = PyObject_CallFunctionObjArgs(callback, single, NULL);
          if (result == NULL) {
              fail = 1;
              break;
          };

          if (single->ob_refcnt != 1) {
            // If the callback saved a reference to the item, don't re-use it.
            Py_XDECREF(single);
            single = NULL;
          }
      }
      if (single != NULL) {
        Py_XDECREF(single);
      }

      delete inputStream;
      delete input;

      if (fail) {
          return NULL;
      } else {
          Py_RETURN_NONE;
      }
  }


  
    
      PyObject *
      fastpb_convertPrimitiveBlockstringtable(const ::google::protobuf::Message &value)
      {
          StringTable *obj = (StringTable *)
              StringTable_new(&StringTableType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveBlock_getstringtable(PrimitiveBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_stringtable()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convertPrimitiveBlockstringtable(
                  self->protobuf->stringtable());

        
    }

    int
    PrimitiveBlock_setstringtable(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_stringtable();
        return 0;
      }

      
        PyObject *value = input;
      

      

        if (!PyType_IsSubtype(value->ob_type, &StringTableType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The stringtable attribute value must be an instance of StringTable");
          return -1;
        }

         // .OSMPBF.StringTable
        ::OSMPBF::StringTable *protoValue =
            ((StringTable *) value)->protobuf;

      

      
        
          self->protobuf->clear_stringtable();
          self->protobuf->mutable_stringtable()->MergeFrom(*protoValue);
        
      

      return 0;
    }
  
    
      PyObject *
      fastpb_convertPrimitiveBlockprimitivegroup(const ::google::protobuf::Message &value)
      {
          PrimitiveGroup *obj = (PrimitiveGroup *)
              PrimitiveGroup_new(&PrimitiveGroupType, NULL, NULL);
          obj->protobuf->MergeFrom(value);
          return (PyObject *)obj;
      }
    

    PyObject *
    PrimitiveBlock_getprimitivegroup(PrimitiveBlock *self, void *closure)
    {
        
          int len = self->protobuf->primitivegroup_size();
          PyObject *tuple = PyTuple_New(len);
          for (int i = 0; i < len; ++i) {
            PyObject *value =
                fastpb_convertPrimitiveBlockprimitivegroup(
                    self->protobuf->primitivegroup(i));
            if (!value) {
              return NULL;
            }
            PyTuple_SetItem(tuple, i, value);
          }
          return tuple;

        
    }

    int
    PrimitiveBlock_setprimitivegroup(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_primitivegroup();
        return 0;
      }

      
        if (PyString_Check(input)) {
          PyErr_SetString(PyExc_TypeError, "The primitivegroup attribute value must be a sequence");
          return -1;
        }
        PyObject *sequence = PySequence_Fast(input, "The primitivegroup attribute value must be a sequence");
        self->protobuf->clear_primitivegroup();
        for (Py_ssize_t i = 0, len = PySequence_Length(sequence); i < len; ++i) {
          PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);

      

      

        if (!PyType_IsSubtype(value->ob_type, &PrimitiveGroupType)) {
          PyErr_SetString(PyExc_TypeError,
                          "The primitivegroup attribute value must be an instance of PrimitiveGroup");
          return -1;
        }

         // .OSMPBF.PrimitiveGroup
        ::OSMPBF::PrimitiveGroup *protoValue =
            ((PrimitiveGroup *) value)->protobuf;

      

      
          
            self->protobuf->add_primitivegroup()->MergeFrom(*protoValue);
          
        }

        Py_XDECREF(sequence);
      

      return 0;
    }
  
    

    PyObject *
    PrimitiveBlock_getgranularity(PrimitiveBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_granularity()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->granularity());

        
    }

    int
    PrimitiveBlock_setgranularity(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_granularity();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The granularity attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_granularity(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    PrimitiveBlock_getlat_offset(PrimitiveBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_lat_offset()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->lat_offset());

        
    }

    int
    PrimitiveBlock_setlat_offset(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lat_offset();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lat_offset attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_lat_offset(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    PrimitiveBlock_getlon_offset(PrimitiveBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_lon_offset()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert3(
                  self->protobuf->lon_offset());

        
    }

    int
    PrimitiveBlock_setlon_offset(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_lon_offset();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int64 protoValue;

        // int64
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else if (PyLong_Check(value)) {
          protoValue = PyLong_AsLongLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The lon_offset attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_lon_offset(protoValue);
        
      

      return 0;
    }
  
    

    PyObject *
    PrimitiveBlock_getdate_granularity(PrimitiveBlock *self, void *closure)
    {
        
          if (! self->protobuf->has_date_granularity()) {
            Py_RETURN_NONE;
          }

          return
              fastpb_convert5(
                  self->protobuf->date_granularity());

        
    }

    int
    PrimitiveBlock_setdate_granularity(PrimitiveBlock *self, PyObject *input, void *closure)
    {
      if (input == NULL || input == Py_None) {
        self->protobuf->clear_date_granularity();
        return 0;
      }

      
        PyObject *value = input;
      

      
        ::google::protobuf::int32 protoValue;

        // int32
        if (PyInt_Check(value)) {
          protoValue = PyInt_AsLong(value);
        } else {
          PyErr_SetString(PyExc_TypeError,
                          "The date_granularity attribute value must be an integer");
          return -1;
        }

      

      
        
          self->protobuf->set_date_granularity(protoValue);
        
      

      return 0;
    }
  

  int
  PrimitiveBlock_init(PrimitiveBlock *self, PyObject *args, PyObject *kwds)
  {
      
        
          PyObject *stringtable = NULL;
        
          PyObject *primitivegroup = NULL;
        
          PyObject *granularity = NULL;
        
          PyObject *lat_offset = NULL;
        
          PyObject *lon_offset = NULL;
        
          PyObject *date_granularity = NULL;
        

        static char *kwlist[] = {
          
            (char *) "stringtable",
          
            (char *) "primitivegroup",
          
            (char *) "granularity",
          
            (char *) "lat_offset",
          
            (char *) "lon_offset",
          
            (char *) "date_granularity",
          
          NULL
        };

        if (! PyArg_ParseTupleAndKeywords(
            args, kwds, "|OOOOOO", kwlist,
            &stringtable,&primitivegroup,&granularity,&lat_offset,&lon_offset,&date_granularity))
          return -1;

        
          if (stringtable) {
            if (PrimitiveBlock_setstringtable(self, stringtable, NULL) < 0) {
              return -1;
            }
          }
        
          if (primitivegroup) {
            if (PrimitiveBlock_setprimitivegroup(self, primitivegroup, NULL) < 0) {
              return -1;
            }
          }
        
          if (granularity) {
            if (PrimitiveBlock_setgranularity(self, granularity, NULL) < 0) {
              return -1;
            }
          }
        
          if (lat_offset) {
            if (PrimitiveBlock_setlat_offset(self, lat_offset, NULL) < 0) {
              return -1;
            }
          }
        
          if (lon_offset) {
            if (PrimitiveBlock_setlon_offset(self, lon_offset, NULL) < 0) {
              return -1;
            }
          }
        
          if (date_granularity) {
            if (PrimitiveBlock_setdate_granularity(self, date_granularity, NULL) < 0) {
              return -1;
            }
          }
        
      

      return 0;
  }


  PyObject *
  PrimitiveBlock_richcompare(PyObject *self, PyObject *other, int op)
  {
      PyObject *result = NULL;
      if (!PyType_IsSubtype(other->ob_type, &PrimitiveBlockType)) {
          result = Py_NotImplemented;
      } else {
          // This is not a particularly efficient implementation since it never short circuits, but it's better
          // than nothing.  It should probably only be used for tests.
          PrimitiveBlock *selfValue = (PrimitiveBlock *)self;
          PrimitiveBlock *otherValue = (PrimitiveBlock *)other;
          std::string selfSerialized;
          std::string otherSerialized;
          Py_BEGIN_ALLOW_THREADS
          selfValue->protobuf->SerializeToString(&selfSerialized);
          otherValue->protobuf->SerializeToString(&otherSerialized);
          Py_END_ALLOW_THREADS

          int cmp = selfSerialized.compare(otherSerialized);
          bool value = false;
          switch (op) {
              case Py_LT:
                  value = cmp < 0;
                  break;
              case Py_LE:
                  value = cmp <= 0;
                  break;
              case Py_EQ:
                  value = cmp == 0;
                  break;
              case Py_NE:
                  value = cmp != 0;
                  break;
              case Py_GT:
                  value = cmp > 0;
                  break;
              case Py_GE:
                  value = cmp >= 0;
                  break;
          }
          result = value ? Py_True : Py_False;
      }

      Py_XINCREF(result);
      return result;
  }


  static PyObject *
  PrimitiveBlock_repr(PyObject *selfObject)
  {
      PrimitiveBlock *self = (PrimitiveBlock *)selfObject;
      PyObject *member;
      PyObject *memberRepr;
      std::stringstream result;
      result << "PrimitiveBlock(";

      
        
        result << "stringtable=";
        member = PrimitiveBlock_getstringtable(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "primitivegroup=";
        member = PrimitiveBlock_getprimitivegroup(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "granularity=";
        member = PrimitiveBlock_getgranularity(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lat_offset=";
        member = PrimitiveBlock_getlat_offset(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "lon_offset=";
        member = PrimitiveBlock_getlon_offset(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      
        
          result << ", ";
        
        result << "date_granularity=";
        member = PrimitiveBlock_getdate_granularity(self, NULL);
        memberRepr = PyObject_Repr(member);
        result << PyString_AsString(memberRepr);
        Py_XDECREF(memberRepr);
        Py_XDECREF(member);
      

      result << ")";

      std::string resultString = result.str();
      return PyUnicode_Decode(resultString.data(), resultString.length(), "utf-8", NULL);
  }


  PyMemberDef PrimitiveBlock_members[] = {
      {NULL}  // Sentinel
  };


  PyGetSetDef PrimitiveBlock_getsetters[] = {
    
      {(char *)"stringtable",
       (getter)PrimitiveBlock_getstringtable, (setter)PrimitiveBlock_setstringtable,
       (char *)"",
       NULL},
    
      {(char *)"primitivegroup",
       (getter)PrimitiveBlock_getprimitivegroup, (setter)PrimitiveBlock_setprimitivegroup,
       (char *)"",
       NULL},
    
      {(char *)"granularity",
       (getter)PrimitiveBlock_getgranularity, (setter)PrimitiveBlock_setgranularity,
       (char *)"",
       NULL},
    
      {(char *)"lat_offset",
       (getter)PrimitiveBlock_getlat_offset, (setter)PrimitiveBlock_setlat_offset,
       (char *)"",
       NULL},
    
      {(char *)"lon_offset",
       (getter)PrimitiveBlock_getlon_offset, (setter)PrimitiveBlock_setlon_offset,
       (char *)"",
       NULL},
    
      {(char *)"date_granularity",
       (getter)PrimitiveBlock_getdate_granularity, (setter)PrimitiveBlock_setdate_granularity,
       (char *)"",
       NULL},
    
      {NULL}  // Sentinel
  };


  PyMethodDef PrimitiveBlock_methods[] = {
      {"DebugString", (PyCFunction)PrimitiveBlock_DebugString, METH_NOARGS,
       "Generates a human readable form of this message, useful for debugging and other purposes."
      },
      {"SerializeToString", (PyCFunction)PrimitiveBlock_SerializeToString, METH_NOARGS,
       "Serializes the protocol buffer to a string."
      },
      {"SerializeMany", (PyCFunction)PrimitiveBlock_SerializeMany, METH_O | METH_CLASS,
       "Serializes a sequence of protocol buffers to a string."
      },
      {"ParseFromString", (PyCFunction)PrimitiveBlock_ParseFromString, METH_O,
       "Parses the protocol buffer from a string."
      },
      {"ParseFromLongString", (PyCFunction)PrimitiveBlock_ParseFromLongString, METH_O,
       "Parses the protocol buffer from a string as large as 512MB."
      },
      {"ParseMany", (PyCFunction)PrimitiveBlock_ParseMany, METH_VARARGS | METH_CLASS,
       "Parses many protocol buffers of this type from a string."
      },
      {NULL}  // Sentinel
  };


  PyTypeObject PrimitiveBlockType = {
      PyObject_HEAD_INIT(NULL)
      0,                                      /*ob_size*/
      "OSMPBF.PrimitiveBlock",  /*tp_name*/
      sizeof(PrimitiveBlock),             /*tp_basicsize*/
      0,                                      /*tp_itemsize*/
      (destructor)PrimitiveBlock_dealloc, /*tp_dealloc*/
      0,                                      /*tp_print*/
      0,                                      /*tp_getattr*/
      0,                                      /*tp_setattr*/
      0,                                      /*tp_compare*/
      PrimitiveBlock_repr,                /*tp_repr*/
      0,                                      /*tp_as_number*/
      0,                                      /*tp_as_sequence*/
      0,                                      /*tp_as_mapping*/
      0,                                      /*tp_hash */
      0,                                      /*tp_call*/
      0,                                      /*tp_str*/
      0,                                      /*tp_getattro*/
      0,                                      /*tp_setattro*/
      0,                                      /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
      "PrimitiveBlock objects",           /* tp_doc */
      0,                                      /* tp_traverse */
      0,                                      /* tp_clear */
      PrimitiveBlock_richcompare,         /* tp_richcompare */
      0,	   	                                /* tp_weaklistoffset */
      0,                   		                /* tp_iter */
      0,		                                  /* tp_iternext */
      PrimitiveBlock_methods,             /* tp_methods */
      PrimitiveBlock_members,             /* tp_members */
      PrimitiveBlock_getsetters,          /* tp_getset */
      0,                                      /* tp_base */
      0,                                      /* tp_dict */
      0,                                      /* tp_descr_get */
      0,                                      /* tp_descr_set */
      0,                                      /* tp_dictoffset */
      (initproc)PrimitiveBlock_init,      /* tp_init */
      0,                                      /* tp_alloc */
      PrimitiveBlock_new,                 /* tp_new */
  };
}



static PyMethodDef module_methods[] = {
    {NULL}  // Sentinel
};

#ifndef PyMODINIT_FUNC	// Declarations for DLL import/export.
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initOSMPBF(void)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    PyObject* m;

    

    
      if (PyType_Ready(&BlobType) < 0)
          return;
    
      if (PyType_Ready(&BlobHeaderType) < 0)
          return;
    
      if (PyType_Ready(&ChangeSetType) < 0)
          return;
    
      if (PyType_Ready(&DenseInfoType) < 0)
          return;
    
      if (PyType_Ready(&HeaderBBoxType) < 0)
          return;
    
      if (PyType_Ready(&InfoType) < 0)
          return;
    
      if (PyType_Ready(&StringTableType) < 0)
          return;
    
      if (PyType_Ready(&DenseNodesType) < 0)
          return;
    
      if (PyType_Ready(&HeaderBlockType) < 0)
          return;
    
      if (PyType_Ready(&NodeType) < 0)
          return;
    
      if (PyType_Ready(&RelationType) < 0)
          return;
    
      if (PyType_Ready(&WayType) < 0)
          return;
    
      if (PyType_Ready(&PrimitiveGroupType) < 0)
          return;
    
      if (PyType_Ready(&PrimitiveBlockType) < 0)
          return;
    

    m = Py_InitModule3("OSMPBF", module_methods,
                       "");

    if (m == NULL)
      return;

    

    
      Py_INCREF(&BlobType);
      PyModule_AddObject(m, "Blob", (PyObject *)&BlobType);
    
      Py_INCREF(&BlobHeaderType);
      PyModule_AddObject(m, "BlobHeader", (PyObject *)&BlobHeaderType);
    
      Py_INCREF(&ChangeSetType);
      PyModule_AddObject(m, "ChangeSet", (PyObject *)&ChangeSetType);
    
      Py_INCREF(&DenseInfoType);
      PyModule_AddObject(m, "DenseInfo", (PyObject *)&DenseInfoType);
    
      Py_INCREF(&HeaderBBoxType);
      PyModule_AddObject(m, "HeaderBBox", (PyObject *)&HeaderBBoxType);
    
      Py_INCREF(&InfoType);
      PyModule_AddObject(m, "Info", (PyObject *)&InfoType);
    
      Py_INCREF(&StringTableType);
      PyModule_AddObject(m, "StringTable", (PyObject *)&StringTableType);
    
      Py_INCREF(&DenseNodesType);
      PyModule_AddObject(m, "DenseNodes", (PyObject *)&DenseNodesType);
    
      Py_INCREF(&HeaderBlockType);
      PyModule_AddObject(m, "HeaderBlock", (PyObject *)&HeaderBlockType);
    
      Py_INCREF(&NodeType);
      PyModule_AddObject(m, "Node", (PyObject *)&NodeType);
    
      Py_INCREF(&RelationType);
      PyModule_AddObject(m, "Relation", (PyObject *)&RelationType);
    
      Py_INCREF(&WayType);
      PyModule_AddObject(m, "Way", (PyObject *)&WayType);
    
      Py_INCREF(&PrimitiveGroupType);
      PyModule_AddObject(m, "PrimitiveGroup", (PyObject *)&PrimitiveGroupType);
    
      Py_INCREF(&PrimitiveBlockType);
      PyModule_AddObject(m, "PrimitiveBlock", (PyObject *)&PrimitiveBlockType);
    
}