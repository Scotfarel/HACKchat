// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: client.proto

#include "client.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace hackchat {
class FromClientDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<FromClient>
      _instance;
} _FromClient_default_instance_;
}  // namespace hackchat
namespace protobuf_client_2eproto {
void InitDefaultsFromClientImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::hackchat::_FromClient_default_instance_;
    new (ptr) ::hackchat::FromClient();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::hackchat::FromClient::InitAsDefaultInstance();
}

void InitDefaultsFromClient() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsFromClientImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::hackchat::FromClient, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::hackchat::FromClient, sender_id_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::hackchat::FromClient, host_id_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::hackchat::FromClient, msg_text_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::hackchat::FromClient, is_feature_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::hackchat::FromClient)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::hackchat::_FromClient_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "client.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\014client.proto\022\010hackchat\"V\n\nFromClient\022\021"
      "\n\tsender_id\030\001 \001(\005\022\017\n\007host_id\030\002 \001(\005\022\020\n\010ms"
      "g_text\030\003 \001(\t\022\022\n\nis_feature\030\004 \001(\010b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 120);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "client.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_client_2eproto
namespace hackchat {

// ===================================================================

void FromClient::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int FromClient::kSenderIdFieldNumber;
const int FromClient::kHostIdFieldNumber;
const int FromClient::kMsgTextFieldNumber;
const int FromClient::kIsFeatureFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

FromClient::FromClient()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_client_2eproto::InitDefaultsFromClient();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:hackchat.FromClient)
}
FromClient::FromClient(const FromClient& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  msg_text_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.msg_text().size() > 0) {
    msg_text_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.msg_text_);
  }
  ::memcpy(&sender_id_, &from.sender_id_,
    static_cast<size_t>(reinterpret_cast<char*>(&is_feature_) -
    reinterpret_cast<char*>(&sender_id_)) + sizeof(is_feature_));
  // @@protoc_insertion_point(copy_constructor:hackchat.FromClient)
}

void FromClient::SharedCtor() {
  msg_text_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&sender_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&is_feature_) -
      reinterpret_cast<char*>(&sender_id_)) + sizeof(is_feature_));
  _cached_size_ = 0;
}

FromClient::~FromClient() {
  // @@protoc_insertion_point(destructor:hackchat.FromClient)
  SharedDtor();
}

void FromClient::SharedDtor() {
  msg_text_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void FromClient::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* FromClient::descriptor() {
  ::protobuf_client_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_client_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const FromClient& FromClient::default_instance() {
  ::protobuf_client_2eproto::InitDefaultsFromClient();
  return *internal_default_instance();
}

FromClient* FromClient::New(::google::protobuf::Arena* arena) const {
  FromClient* n = new FromClient;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void FromClient::Clear() {
// @@protoc_insertion_point(message_clear_start:hackchat.FromClient)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  msg_text_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&sender_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&is_feature_) -
      reinterpret_cast<char*>(&sender_id_)) + sizeof(is_feature_));
  _internal_metadata_.Clear();
}

bool FromClient::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:hackchat.FromClient)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 sender_id = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &sender_id_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 host_id = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(16u /* 16 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &host_id_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string msg_text = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(26u /* 26 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_msg_text()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->msg_text().data(), static_cast<int>(this->msg_text().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "hackchat.FromClient.msg_text"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bool is_feature = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(32u /* 32 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &is_feature_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:hackchat.FromClient)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:hackchat.FromClient)
  return false;
#undef DO_
}

void FromClient::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:hackchat.FromClient)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 sender_id = 1;
  if (this->sender_id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->sender_id(), output);
  }

  // int32 host_id = 2;
  if (this->host_id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->host_id(), output);
  }

  // string msg_text = 3;
  if (this->msg_text().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->msg_text().data(), static_cast<int>(this->msg_text().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "hackchat.FromClient.msg_text");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->msg_text(), output);
  }

  // bool is_feature = 4;
  if (this->is_feature() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(4, this->is_feature(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:hackchat.FromClient)
}

::google::protobuf::uint8* FromClient::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:hackchat.FromClient)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 sender_id = 1;
  if (this->sender_id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->sender_id(), target);
  }

  // int32 host_id = 2;
  if (this->host_id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->host_id(), target);
  }

  // string msg_text = 3;
  if (this->msg_text().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->msg_text().data(), static_cast<int>(this->msg_text().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "hackchat.FromClient.msg_text");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->msg_text(), target);
  }

  // bool is_feature = 4;
  if (this->is_feature() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(4, this->is_feature(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:hackchat.FromClient)
  return target;
}

size_t FromClient::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:hackchat.FromClient)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string msg_text = 3;
  if (this->msg_text().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->msg_text());
  }

  // int32 sender_id = 1;
  if (this->sender_id() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->sender_id());
  }

  // int32 host_id = 2;
  if (this->host_id() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->host_id());
  }

  // bool is_feature = 4;
  if (this->is_feature() != 0) {
    total_size += 1 + 1;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void FromClient::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:hackchat.FromClient)
  GOOGLE_DCHECK_NE(&from, this);
  const FromClient* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const FromClient>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:hackchat.FromClient)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:hackchat.FromClient)
    MergeFrom(*source);
  }
}

void FromClient::MergeFrom(const FromClient& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:hackchat.FromClient)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.msg_text().size() > 0) {

    msg_text_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.msg_text_);
  }
  if (from.sender_id() != 0) {
    set_sender_id(from.sender_id());
  }
  if (from.host_id() != 0) {
    set_host_id(from.host_id());
  }
  if (from.is_feature() != 0) {
    set_is_feature(from.is_feature());
  }
}

void FromClient::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:hackchat.FromClient)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FromClient::CopyFrom(const FromClient& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:hackchat.FromClient)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FromClient::IsInitialized() const {
  return true;
}

void FromClient::Swap(FromClient* other) {
  if (other == this) return;
  InternalSwap(other);
}
void FromClient::InternalSwap(FromClient* other) {
  using std::swap;
  msg_text_.Swap(&other->msg_text_);
  swap(sender_id_, other->sender_id_);
  swap(host_id_, other->host_id_);
  swap(is_feature_, other->is_feature_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata FromClient::GetMetadata() const {
  protobuf_client_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_client_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace hackchat

// @@protoc_insertion_point(global_scope)