#include <boost/python.hpp>
#include <CsMerge.hpp>


namespace py = boost::python;
using namespace csmerge;


struct CsTokenFromPythonNumber {
    static void* convertible(PyObject* objPtr) {
        if (!PyLong_Check(objPtr) && !PyFloat_Check(objPtr)) {
            return NULL;
        }

        return objPtr;
    }

    static void construct(PyObject* objPtr,
        py::converter::rvalue_from_python_stage1_data* data) {

        typedef py::converter::rvalue_from_python_storage<CsToken> rval_t;

        double value = PyFloat_AsDouble(objPtr);

        void* storage = reinterpret_cast<rval_t*>(data)->storage.bytes;
        new (storage) CsToken(value);

        data->convertible = storage;
    }
};


struct CsTokenFromPythonString {
    static void* convertible(PyObject* objPtr) {
        if (!PyUnicode_Check(objPtr)) {
            return NULL;
        }

        return objPtr;
    }

    static void construct(PyObject* objPtr,
        py::converter::rvalue_from_python_stage1_data* data) {

        typedef py::converter::rvalue_from_python_storage<CsToken> rval_t;

        long int size;
        const char* value = PyUnicode_AsUTF8AndSize(objPtr, &size);
        assert(value);

        void* storage = reinterpret_cast<rval_t*>(data)->storage.bytes;
        new (storage) CsToken(std::string(value, size));

        data->convertible = storage;
    }
};


static py::list mergeCharstrings_helper(py::list& cs1Tokens, py::list& cs2Tokens) {
    Charstring cs1;
    for (int i = 0; i < py::len(cs1Tokens); ++i) {
        cs1.push_back(py::extract<CsToken>(cs1Tokens[i]));
    }

    Charstring cs2;
    for (int i = 0; i < py::len(cs2Tokens); ++i) {
        cs2.push_back(py::extract<CsToken>(cs2Tokens[i]));
    }

    Charstring merged = mergeCharstrings(cs1, cs2);

    py::list result;

    for (const CsToken& tok : merged) {
        switch (tok.type) {
            case PS_OPERATOR:
                result.append(tok.str);
                break;
            case PS_OPERAND:
                result.append(tok.num);
                break;
        }
    }

    return result;
}

static void translateException(const CsMergeException& ex) {
    PyErr_SetString(PyExc_RuntimeWarning, ex.what());
}

static void setFloatPrecision(double fp) {
    geometry::FLOAT_PRECISION = fp;
}

static double getFloatPrecision() {
    return geometry::FLOAT_PRECISION;
}

static void setMinLsegLength(double len) {
    geometry::MIN_LSEG_LENGTH = len;
}

static double getMinLsegLength() {
    return geometry::MIN_LSEG_LENGTH;
}

static void setMaxLsegsPerBezier(int n) {
    geometry::MAX_LSEGS_PER_BEZIER = n;
}

static int getMaxLsegsPerBezier() {
    return geometry::MAX_LSEGS_PER_BEZIER;
}

BOOST_PYTHON_MODULE(_pycsmerge) {
    py::def("initialise", &csmerge::initialise);
    py::def("merge_charstrings", &mergeCharstrings_helper);
    py::def("set_float_precision", &setFloatPrecision);
    py::def("get_float_precision", &getFloatPrecision);
    py::def("set_min_lseg_length", &setMinLsegLength);
    py::def("get_min_lseg_length", &getMinLsegLength);
    py::def("set_max_lsegs_per_bezier", &setMaxLsegsPerBezier);
    py::def("get_max_lsegs_per_bezier", &getMaxLsegsPerBezier);

    py::class_<CsToken>("CsToken", py::init<double>())
        .def(py::init<const std::string&>())
        .def("__eq__", &CsToken::operator==)
        .def("__ne__", &CsToken::operator!=);

    py::converter::registry::push_back(
        &CsTokenFromPythonNumber::convertible,
        &CsTokenFromPythonNumber::construct,
        py::type_id<CsToken>());

    py::converter::registry::push_back(
        &CsTokenFromPythonString::convertible,
        &CsTokenFromPythonString::construct,
        py::type_id<CsToken>());

    py::register_exception_translator<CsMergeException>(&translateException);
}
