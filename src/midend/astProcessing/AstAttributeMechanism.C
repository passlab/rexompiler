#include "sage3basic.h"
#include "AstAttributeMechanism.h"

#include "roseInternal.h"
#include <sstream>
#include <cstddef>
#include <cstring>

using namespace Rose;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttributeMechanism
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
attributeFullName(const std::string & name, AstAttribute *value) {
    std::string retval;

    if (value == NULL) {
        retval = "null attribute";
    } else {
        std::string className = value->attribute_class_name();
        retval = "attribute " + name + "(" + value->toString() + ") of " + className + " type";
    }
    return retval;
}

static void
deleteAttributeValue(const std::string &name, AstAttribute *value) {
    if (value != NULL) {
        switch (value->getOwnershipPolicy()) {
            case AstAttribute::CONTAINER_OWNERSHIP:
                delete value;
                break;
            case AstAttribute::NO_OWNERSHIP:
                MLOG_WARN_CXX("midend:astProcessing") << attributeFullName(name, value) << "is leaked\n";
                break;
            case AstAttribute::CUSTOM_OWNERSHIP:
                // assume ownership is properly implemented by the subclass
                break;
            case AstAttribute::UNKNOWN_OWNERSHIP: {
                MLOG_WARN_CXX("midend:astProcessing") << "attribute " << attributeFullName(name, value) << "ownership is unknown and possibly leaked\n";

                // Show details about how to fix this only once per program.
                static bool detailsShown = false;
                if (!detailsShown) {
                    MLOG_WARN_CXX("midend:astProcessing") <<"    This attribute's class should include a definition for the virtual\n"
                               <<"    \"getOwnershipPolicy\" based on the intention the author had for how\n"
                               <<"    memory is managed for this attribute type.  If the author intended\n"
                               <<"    attribute memory to be managed by ROSE's AstAttributeMechanism, then\n"
                               <<"    the following definition should be added to the attribute's class:\n"
                               <<"        virtual AstAttribute::OwnershipPolicy\n"
                               <<"        getOwnershipPolicy() const override {\n"
                               <<"            return CONTAINER_OWNERSHIP;\n"
                               <<"        }\n"
                               <<"    and the attribute should not be explicitly deleted by the attribute\n"
                               <<"    creator or any users of the attribute. See documentation for\n"
                               <<"    AstAttribute::getOwnershipPolicy for details.\n";
                    detailsShown = true;
                }
                break;
            }
        }
    }
}

AstAttributeMechanism&
AstAttributeMechanism::operator=(const AstAttributeMechanism &other) {
    assignFrom(other);
    return *this;
}

AstAttributeMechanism::~AstAttributeMechanism() {
    for(auto it=attributes_.begin(); it!=attributes_.end(); it++) {
        deleteAttributeValue(it->first, it->second);
		attributes_.erase(it);
    }
}

bool
AstAttributeMechanism::exists(const std::string &name) const {
	if(attributes_.find(name) != attributes_.end()) return true;
	else return false;
}

void
AstAttributeMechanism::set(const std::string &name, AstAttribute *newValue) {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) { //already continue the key
    	AstAttribute * oldValue = (*it).second;
    	deleteAttributeValue(name, oldValue);
        if (newValue != NULL) {
        	(*it).second = newValue;
        }
        else attributes_.erase(it);
    } else if (newValue != NULL) {
        attributes_.insert(std::make_pair(name, newValue));
    }
}

// insert if not already existing
bool
AstAttributeMechanism::add(const std::string &name, AstAttribute *value) {
	auto it = attributes_.find(name);
	if (it != attributes_.end()) { return false; }
	else if (value != NULL) {
        attributes_.insert(std::make_pair(name, value));
        return true;
    } return false;
}

// insert only if already existing
bool
AstAttributeMechanism::replace(const std::string &name, AstAttribute *value) {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) { //already continue the key
    	AstAttribute * oldValue = (*it).second;
    	deleteAttributeValue(name, oldValue);
        if (value != NULL) (*it).second = value;
        else attributes_.erase(it);
        return true;
    }
    return false;
}

AstAttribute*
AstAttributeMechanism::operator[](const std::string &name) const {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) { //already continue the key
        return (*it).second;
    }
    return NULL;
}

// erase
void
AstAttributeMechanism::remove(const std::string &name) {
    attributeMap_::iterator it = attributes_.find(name);
    if (it != attributes_.end()) { //already continue the key
    	AstAttribute * oldValue = (*it).second;
    	deleteAttributeValue(name, oldValue);
    	attributes_.erase(it);
    }
}

// get attribute names
AstAttributeMechanism::AttributeIdentifiers
AstAttributeMechanism::getAttributeIdentifiers() const {
    AttributeIdentifiers retval;
    for(auto it=attributes_.begin(); it!=attributes_.end(); it++)
        retval.insert(it->first);
    return retval;
}

size_t
AstAttributeMechanism::size() const {
    return attributes_.size();
}

// Construction and assignment. Must be exception-safe.
void
AstAttributeMechanism::assignFrom(const AstAttributeMechanism &other) {
    if (this == &other) return;
    AstAttributeMechanism tmp;                          // for exception safety
    for(auto it=attributes_.begin(); it!=attributes_.end(); it++) {
        /*!const*/ AstAttribute *attr = other[it->first];
        ASSERT_not_null(attr);

        // Copy the attribute. This might throw, which is why we're using "tmp". If it throws, then we don't ever make it to
        // the std::swap below, and the destination is unchanged and tmp will be cleaned up according to the ownership policy
        // of each attribute. Also, as mentioned in the header file, be sure we invoke the non-const version of "copy" for the
        // sake of subclasses that have not been modified to define the const version.
        AstAttribute *copied = attr->copy();

        // Check various things about the attribute.  Some of these might be important in places other than copying, but we
        // report them here because there's a large amount of user code that doesn't follow the rules and we don't want to be
        // completely obnoxious.
        if (!copied)
        	MLOG_WARN_CXX("midend:astProcessing") << attributeFullName(it->first, it->second) <<" was not copied; no virtual copy function?\n";
        if (attr->attribute_class_name().compare("AstAttribute") == 0) {
        	MLOG_WARN_CXX("midend:astProcessing") << attributeFullName(it->first, it->second) <<"\""
                       <<" does not implement attribute_class_name\n";
        }
        switch (attr->getOwnershipPolicy()) {
            case AstAttribute::CONTAINER_OWNERSHIP:
                ASSERT_require2(copied != attr, "virtual copy function for \"%s\" did not copy attribute\n", attributeFullName(it->first, it->second).c_str());
                break;
            case AstAttribute::NO_OWNERSHIP:
                // copy() is allowed to return itself; we don't care since we won't ever delete either attribute
                // A leak will be reported instead of deleting the attribute.
                break;
            case AstAttribute::CUSTOM_OWNERSHIP:
                // copy() is allowed to return itself; we don't care since we won't ever delete either attribute
                // Assume attribute ownership is properly implemented without leaking memory.
                break;
            case AstAttribute::UNKNOWN_OWNERSHIP:
                // Similar to CONTAINER_OWNERSHIP but only warn, don't assert.
                if (copied != attr)
                	MLOG_WARN_CXX("midend:astProcessing") <<attributeFullName(it->first, it->second) <<" virtual copy function did not copy the attribute (unknown ownership)\n";
                break;
        }

        if (copied) tmp.set(it->first, copied);
    }
    std::swap(attributes_, tmp.attributes_);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstAttribute::OwnershipPolicy
AstAttribute::getOwnershipPolicy() const {
    return UNKNOWN_OWNERSHIP;
}

std::string
AstAttribute::toString() {
    return StringUtility::numberToString((void*)(this));
}

int
AstAttribute::packed_size() {
    return 0;
}

char*
AstAttribute::packed_data() {
    return NULL;
}

void
AstAttribute::unpacked_data(int size, char* data) {}

std::string
AstAttribute::additionalNodeOptions() {
    return "";
}

std::vector<AstAttribute::AttributeEdgeInfo>
AstAttribute::additionalEdgeInfo() {
    return std::vector<AttributeEdgeInfo>();
}

std::vector<AstAttribute::AttributeNodeInfo>
AstAttribute::additionalNodeInfo() {
    return std::vector<AttributeNodeInfo>();
}

bool
AstAttribute::commentOutNodeInGraph() {
    return false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MetricAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstAttribute*
MetricAttribute::copy() const {
    return new MetricAttribute(*this);
}

std::string
MetricAttribute::attribute_class_name() const {
    return "MetricAttribute";
}

MetricAttribute&
MetricAttribute::operator+=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ += other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator-=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ -= other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator*=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ *= other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator/=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ /= other.value_;
    return *this;
}

std::string
MetricAttribute::toString() {
    std::ostringstream ostr;
    ostr << value_;
    std::string retval = ostr.str();

    if (is_derived_)
        retval += "**";
    return retval;
}

int
MetricAttribute::packed_size() {
    return toString().size()+1;                         // +1 because of NUL-character
}

char*
MetricAttribute::packed_data() {
    // Reimplemented because old version returned a dangling pointer. [Robb Matzke 2015-11-10]
    std::string str = toString();
    static char buf[256];
    ASSERT_require(str.size() < sizeof buf);            // remember NUL terminator
    memcpy(buf, str.c_str(), str.size()+1);
    return buf;
}

void
MetricAttribute::unpacked_data(int size, char* data) {
    if (size <= 0)
        return;

    ROSE_ASSERT(data != NULL);

    // check tail **
    char * head = data;

 // char * tail = head + strlen(head) - 1;
    int string_size_minus_one = strlen(head) - 1;
    if (string_size_minus_one >= 0) {
       char * tail = head + string_size_minus_one;
       if (*tail=='*')
          is_derived_ = true;

    // retrieve value
    // strtod() is smart enough to skip tab and ignore tailing **
       char * endp = NULL;
       value_=strtod(head,&endp);
    }
    else
      {
        std::cout << "Error: In MetricAttribute::unpacked_data(): data string is zero length" << std::endl;
        ROSE_ABORT();
      }
}
