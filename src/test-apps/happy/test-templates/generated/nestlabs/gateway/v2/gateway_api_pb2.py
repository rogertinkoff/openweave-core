# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: nestlabs/gateway/v2/gateway_api.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.protobuf import any_pb2 as google_dot_protobuf_dot_any__pb2
from google.protobuf import field_mask_pb2 as google_dot_protobuf_dot_field__mask__pb2
from google.protobuf import timestamp_pb2 as google_dot_protobuf_dot_timestamp__pb2
from nest.messages import schema_version_pb2 as nest_dot_messages_dot_schema__version__pb2
from nestlabs.gateway.v1 import trait_api_pb2 as nestlabs_dot_gateway_dot_v1_dot_trait__api__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='nestlabs/gateway/v2/gateway_api.proto',
  package='nestlabs.gateway.v2',
  syntax='proto3',
  serialized_pb=_b('\n%nestlabs/gateway/v2/gateway_api.proto\x12\x13nestlabs.gateway.v2\x1a\x19google/protobuf/any.proto\x1a google/protobuf/field_mask.proto\x1a\x1fgoogle/protobuf/timestamp.proto\x1a\"nest/messages/schema_version.proto\x1a#nestlabs/gateway/v1/trait_api.proto\"d\n\tTraitMeta\x12\x13\n\x0btrait_label\x18\x01 \x01(\t\x12\x0c\n\x04type\x18\x02 \x01(\t\x12\x34\n\x0eschema_version\x18\x03 \x01(\x0b\x32\x1c.nest.messages.SchemaVersion\"\xf2\x01\n\tIfaceMeta\x12\x13\n\x0biface_label\x18\x01 \x01(\t\x12\x0c\n\x04type\x18\x02 \x01(\t\x12R\n\x13trait_label_mapping\x18\x03 \x03(\x0b\x32\x35.nestlabs.gateway.v2.IfaceMeta.TraitLabelMappingEntry\x12\x34\n\x0eschema_version\x18\x04 \x01(\x0b\x32\x1c.nest.messages.SchemaVersion\x1a\x38\n\x16TraitLabelMappingEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t:\x02\x38\x01\"\xf6\x01\n\x0cResourceMeta\x12\x13\n\x0bresource_id\x18\x01 \x01(\t\x12\x0c\n\x04type\x18\x02 \x01(\t\x12\x33\n\x06status\x18\x03 \x01(\x0e\x32#.nestlabs.gateway.v2.ResourceStatus\x12\x33\n\x0btrait_metas\x18\x04 \x03(\x0b\x32\x1e.nestlabs.gateway.v2.TraitMeta\x12\x1e\n\x16\x63urrent_schema_version\x18\x06 \x01(\r\x12\x33\n\x0biface_metas\x18\x07 \x03(\x0b\x32\x1e.nestlabs.gateway.v2.IfaceMetaJ\x04\x08\x05\x10\x06\"3\n\x07TraitId\x12\x13\n\x0bresource_id\x18\x01 \x01(\t\x12\x13\n\x0btrait_label\x18\x02 \x01(\t\"3\n\x05Patch\x12$\n\x06values\x18\x01 \x01(\x0b\x32\x14.google.protobuf.AnyJ\x04\x08\x02\x10\x03\"\xad\x01\n\nTraitState\x12.\n\x08trait_id\x18\x01 \x01(\x0b\x32\x1c.nestlabs.gateway.v2.TraitId\x12\x33\n\x0bstate_types\x18\x02 \x03(\x0e\x32\x1e.nestlabs.gateway.v2.StateType\x12)\n\x05patch\x18\x03 \x01(\x0b\x32\x1a.nestlabs.gateway.v2.Patch\x12\x0f\n\x07version\x18\x04 \x01(\x04\"\x83\x01\n\x16TraitTypeObserveParams\x12\x12\n\ntrait_type\x18\x01 \x01(\t\x12\x34\n\x10state_field_mask\x18\x02 \x01(\x0b\x32\x1a.google.protobuf.FieldMask\x12\x1f\n\x17observer_schema_version\x18\x03 \x01(\r\"e\n\x1aTraitInstanceObserveParams\x12.\n\x08trait_id\x18\x01 \x01(\x0b\x32\x1c.nestlabs.gateway.v2.TraitId\x12\x17\n\x0f\x63\x61\x63hed_versions\x18\x02 \x03(\x04\"\x84\x02\n\x0eObserveRequest\x12\x33\n\x0bstate_types\x18\x01 \x03(\x0e\x32\x1e.nestlabs.gateway.v2.StateType\x12\x14\n\x0cresource_ids\x18\x02 \x03(\t\x12\x46\n\x11trait_type_params\x18\x03 \x03(\x0b\x32+.nestlabs.gateway.v2.TraitTypeObserveParams\x12N\n\x15trait_instance_params\x18\x04 \x03(\x0b\x32/.nestlabs.gateway.v2.TraitInstanceObserveParams\x12\x0f\n\x07user_id\x18\x05 \x01(\t\"\x83\x01\n\x12TraitOperationList\x12.\n\x08trait_id\x18\x01 \x01(\x0b\x32\x1c.nestlabs.gateway.v2.TraitId\x12=\n\x10trait_operations\x18\x02 \x03(\x0b\x32#.nestlabs.gateway.v1.TraitOperation\"\xa6\x02\n\x0fObserveResponse\x12\x39\n\x0eresource_metas\x18\x01 \x03(\x0b\x32!.nestlabs.gateway.v2.ResourceMeta\x12\'\n\x1finitial_resource_metas_continue\x18\x02 \x01(\x08\x12\x35\n\x0ctrait_states\x18\x03 \x03(\x0b\x32\x1f.nestlabs.gateway.v2.TraitState\x12\x46\n\x15trait_operation_lists\x18\x04 \x03(\x0b\x32\'.nestlabs.gateway.v2.TraitOperationList\x12\x30\n\x0c\x63urrent_time\x18\x05 \x01(\x0b\x32\x1a.google.protobuf.Timestamp*4\n\x0eResourceStatus\x12\n\n\x06NORMAL\x10\x00\x12\t\n\x05\x41\x44\x44\x45\x44\x10\x01\x12\x0b\n\x07REMOVED\x10\x02*D\n\tStateType\x12\x1a\n\x16STATE_TYPE_UNSPECIFIED\x10\x00\x12\r\n\tCONFIRMED\x10\x01\x12\x0c\n\x08\x41\x43\x43\x45PTED\x10\x02\x32h\n\x0eGatewayService\x12V\n\x07Observe\x12#.nestlabs.gateway.v2.ObserveRequest\x1a$.nestlabs.gateway.v2.ObserveResponse0\x01\x42!\n\x17\x63om.nestlabs.gateway.v2P\x01\xa2\x02\x03PCLb\x06proto3')
  ,
  dependencies=[google_dot_protobuf_dot_any__pb2.DESCRIPTOR,google_dot_protobuf_dot_field__mask__pb2.DESCRIPTOR,google_dot_protobuf_dot_timestamp__pb2.DESCRIPTOR,nest_dot_messages_dot_schema__version__pb2.DESCRIPTOR,nestlabs_dot_gateway_dot_v1_dot_trait__api__pb2.DESCRIPTOR,])

_RESOURCESTATUS = _descriptor.EnumDescriptor(
  name='ResourceStatus',
  full_name='nestlabs.gateway.v2.ResourceStatus',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='NORMAL', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ADDED', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='REMOVED', index=2, number=2,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=2038,
  serialized_end=2090,
)
_sym_db.RegisterEnumDescriptor(_RESOURCESTATUS)

ResourceStatus = enum_type_wrapper.EnumTypeWrapper(_RESOURCESTATUS)
_STATETYPE = _descriptor.EnumDescriptor(
  name='StateType',
  full_name='nestlabs.gateway.v2.StateType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='STATE_TYPE_UNSPECIFIED', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CONFIRMED', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ACCEPTED', index=2, number=2,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=2092,
  serialized_end=2160,
)
_sym_db.RegisterEnumDescriptor(_STATETYPE)

StateType = enum_type_wrapper.EnumTypeWrapper(_STATETYPE)
NORMAL = 0
ADDED = 1
REMOVED = 2
STATE_TYPE_UNSPECIFIED = 0
CONFIRMED = 1
ACCEPTED = 2



_TRAITMETA = _descriptor.Descriptor(
  name='TraitMeta',
  full_name='nestlabs.gateway.v2.TraitMeta',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trait_label', full_name='nestlabs.gateway.v2.TraitMeta.trait_label', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='type', full_name='nestlabs.gateway.v2.TraitMeta.type', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='schema_version', full_name='nestlabs.gateway.v2.TraitMeta.schema_version', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=229,
  serialized_end=329,
)


_IFACEMETA_TRAITLABELMAPPINGENTRY = _descriptor.Descriptor(
  name='TraitLabelMappingEntry',
  full_name='nestlabs.gateway.v2.IfaceMeta.TraitLabelMappingEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='nestlabs.gateway.v2.IfaceMeta.TraitLabelMappingEntry.key', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='value', full_name='nestlabs.gateway.v2.IfaceMeta.TraitLabelMappingEntry.value', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=_descriptor._ParseOptions(descriptor_pb2.MessageOptions(), _b('8\001')),
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=518,
  serialized_end=574,
)

_IFACEMETA = _descriptor.Descriptor(
  name='IfaceMeta',
  full_name='nestlabs.gateway.v2.IfaceMeta',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='iface_label', full_name='nestlabs.gateway.v2.IfaceMeta.iface_label', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='type', full_name='nestlabs.gateway.v2.IfaceMeta.type', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_label_mapping', full_name='nestlabs.gateway.v2.IfaceMeta.trait_label_mapping', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='schema_version', full_name='nestlabs.gateway.v2.IfaceMeta.schema_version', index=3,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_IFACEMETA_TRAITLABELMAPPINGENTRY, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=332,
  serialized_end=574,
)


_RESOURCEMETA = _descriptor.Descriptor(
  name='ResourceMeta',
  full_name='nestlabs.gateway.v2.ResourceMeta',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='resource_id', full_name='nestlabs.gateway.v2.ResourceMeta.resource_id', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='type', full_name='nestlabs.gateway.v2.ResourceMeta.type', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='status', full_name='nestlabs.gateway.v2.ResourceMeta.status', index=2,
      number=3, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_metas', full_name='nestlabs.gateway.v2.ResourceMeta.trait_metas', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='current_schema_version', full_name='nestlabs.gateway.v2.ResourceMeta.current_schema_version', index=4,
      number=6, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='iface_metas', full_name='nestlabs.gateway.v2.ResourceMeta.iface_metas', index=5,
      number=7, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=577,
  serialized_end=823,
)


_TRAITID = _descriptor.Descriptor(
  name='TraitId',
  full_name='nestlabs.gateway.v2.TraitId',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='resource_id', full_name='nestlabs.gateway.v2.TraitId.resource_id', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_label', full_name='nestlabs.gateway.v2.TraitId.trait_label', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=825,
  serialized_end=876,
)


_PATCH = _descriptor.Descriptor(
  name='Patch',
  full_name='nestlabs.gateway.v2.Patch',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='values', full_name='nestlabs.gateway.v2.Patch.values', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=878,
  serialized_end=929,
)


_TRAITSTATE = _descriptor.Descriptor(
  name='TraitState',
  full_name='nestlabs.gateway.v2.TraitState',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trait_id', full_name='nestlabs.gateway.v2.TraitState.trait_id', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='state_types', full_name='nestlabs.gateway.v2.TraitState.state_types', index=1,
      number=2, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='patch', full_name='nestlabs.gateway.v2.TraitState.patch', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='version', full_name='nestlabs.gateway.v2.TraitState.version', index=3,
      number=4, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=932,
  serialized_end=1105,
)


_TRAITTYPEOBSERVEPARAMS = _descriptor.Descriptor(
  name='TraitTypeObserveParams',
  full_name='nestlabs.gateway.v2.TraitTypeObserveParams',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trait_type', full_name='nestlabs.gateway.v2.TraitTypeObserveParams.trait_type', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='state_field_mask', full_name='nestlabs.gateway.v2.TraitTypeObserveParams.state_field_mask', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='observer_schema_version', full_name='nestlabs.gateway.v2.TraitTypeObserveParams.observer_schema_version', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1108,
  serialized_end=1239,
)


_TRAITINSTANCEOBSERVEPARAMS = _descriptor.Descriptor(
  name='TraitInstanceObserveParams',
  full_name='nestlabs.gateway.v2.TraitInstanceObserveParams',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trait_id', full_name='nestlabs.gateway.v2.TraitInstanceObserveParams.trait_id', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='cached_versions', full_name='nestlabs.gateway.v2.TraitInstanceObserveParams.cached_versions', index=1,
      number=2, type=4, cpp_type=4, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1241,
  serialized_end=1342,
)


_OBSERVEREQUEST = _descriptor.Descriptor(
  name='ObserveRequest',
  full_name='nestlabs.gateway.v2.ObserveRequest',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='state_types', full_name='nestlabs.gateway.v2.ObserveRequest.state_types', index=0,
      number=1, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='resource_ids', full_name='nestlabs.gateway.v2.ObserveRequest.resource_ids', index=1,
      number=2, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_type_params', full_name='nestlabs.gateway.v2.ObserveRequest.trait_type_params', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_instance_params', full_name='nestlabs.gateway.v2.ObserveRequest.trait_instance_params', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='user_id', full_name='nestlabs.gateway.v2.ObserveRequest.user_id', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1345,
  serialized_end=1605,
)


_TRAITOPERATIONLIST = _descriptor.Descriptor(
  name='TraitOperationList',
  full_name='nestlabs.gateway.v2.TraitOperationList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trait_id', full_name='nestlabs.gateway.v2.TraitOperationList.trait_id', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_operations', full_name='nestlabs.gateway.v2.TraitOperationList.trait_operations', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1608,
  serialized_end=1739,
)


_OBSERVERESPONSE = _descriptor.Descriptor(
  name='ObserveResponse',
  full_name='nestlabs.gateway.v2.ObserveResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='resource_metas', full_name='nestlabs.gateway.v2.ObserveResponse.resource_metas', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='initial_resource_metas_continue', full_name='nestlabs.gateway.v2.ObserveResponse.initial_resource_metas_continue', index=1,
      number=2, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_states', full_name='nestlabs.gateway.v2.ObserveResponse.trait_states', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trait_operation_lists', full_name='nestlabs.gateway.v2.ObserveResponse.trait_operation_lists', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='current_time', full_name='nestlabs.gateway.v2.ObserveResponse.current_time', index=4,
      number=5, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1742,
  serialized_end=2036,
)

_TRAITMETA.fields_by_name['schema_version'].message_type = nest_dot_messages_dot_schema__version__pb2._SCHEMAVERSION
_IFACEMETA_TRAITLABELMAPPINGENTRY.containing_type = _IFACEMETA
_IFACEMETA.fields_by_name['trait_label_mapping'].message_type = _IFACEMETA_TRAITLABELMAPPINGENTRY
_IFACEMETA.fields_by_name['schema_version'].message_type = nest_dot_messages_dot_schema__version__pb2._SCHEMAVERSION
_RESOURCEMETA.fields_by_name['status'].enum_type = _RESOURCESTATUS
_RESOURCEMETA.fields_by_name['trait_metas'].message_type = _TRAITMETA
_RESOURCEMETA.fields_by_name['iface_metas'].message_type = _IFACEMETA
_PATCH.fields_by_name['values'].message_type = google_dot_protobuf_dot_any__pb2._ANY
_TRAITSTATE.fields_by_name['trait_id'].message_type = _TRAITID
_TRAITSTATE.fields_by_name['state_types'].enum_type = _STATETYPE
_TRAITSTATE.fields_by_name['patch'].message_type = _PATCH
_TRAITTYPEOBSERVEPARAMS.fields_by_name['state_field_mask'].message_type = google_dot_protobuf_dot_field__mask__pb2._FIELDMASK
_TRAITINSTANCEOBSERVEPARAMS.fields_by_name['trait_id'].message_type = _TRAITID
_OBSERVEREQUEST.fields_by_name['state_types'].enum_type = _STATETYPE
_OBSERVEREQUEST.fields_by_name['trait_type_params'].message_type = _TRAITTYPEOBSERVEPARAMS
_OBSERVEREQUEST.fields_by_name['trait_instance_params'].message_type = _TRAITINSTANCEOBSERVEPARAMS
_TRAITOPERATIONLIST.fields_by_name['trait_id'].message_type = _TRAITID
_TRAITOPERATIONLIST.fields_by_name['trait_operations'].message_type = nestlabs_dot_gateway_dot_v1_dot_trait__api__pb2._TRAITOPERATION
_OBSERVERESPONSE.fields_by_name['resource_metas'].message_type = _RESOURCEMETA
_OBSERVERESPONSE.fields_by_name['trait_states'].message_type = _TRAITSTATE
_OBSERVERESPONSE.fields_by_name['trait_operation_lists'].message_type = _TRAITOPERATIONLIST
_OBSERVERESPONSE.fields_by_name['current_time'].message_type = google_dot_protobuf_dot_timestamp__pb2._TIMESTAMP
DESCRIPTOR.message_types_by_name['TraitMeta'] = _TRAITMETA
DESCRIPTOR.message_types_by_name['IfaceMeta'] = _IFACEMETA
DESCRIPTOR.message_types_by_name['ResourceMeta'] = _RESOURCEMETA
DESCRIPTOR.message_types_by_name['TraitId'] = _TRAITID
DESCRIPTOR.message_types_by_name['Patch'] = _PATCH
DESCRIPTOR.message_types_by_name['TraitState'] = _TRAITSTATE
DESCRIPTOR.message_types_by_name['TraitTypeObserveParams'] = _TRAITTYPEOBSERVEPARAMS
DESCRIPTOR.message_types_by_name['TraitInstanceObserveParams'] = _TRAITINSTANCEOBSERVEPARAMS
DESCRIPTOR.message_types_by_name['ObserveRequest'] = _OBSERVEREQUEST
DESCRIPTOR.message_types_by_name['TraitOperationList'] = _TRAITOPERATIONLIST
DESCRIPTOR.message_types_by_name['ObserveResponse'] = _OBSERVERESPONSE
DESCRIPTOR.enum_types_by_name['ResourceStatus'] = _RESOURCESTATUS
DESCRIPTOR.enum_types_by_name['StateType'] = _STATETYPE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

TraitMeta = _reflection.GeneratedProtocolMessageType('TraitMeta', (_message.Message,), dict(
  DESCRIPTOR = _TRAITMETA,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitMeta)
  ))
_sym_db.RegisterMessage(TraitMeta)

IfaceMeta = _reflection.GeneratedProtocolMessageType('IfaceMeta', (_message.Message,), dict(

  TraitLabelMappingEntry = _reflection.GeneratedProtocolMessageType('TraitLabelMappingEntry', (_message.Message,), dict(
    DESCRIPTOR = _IFACEMETA_TRAITLABELMAPPINGENTRY,
    __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
    # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.IfaceMeta.TraitLabelMappingEntry)
    ))
  ,
  DESCRIPTOR = _IFACEMETA,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.IfaceMeta)
  ))
_sym_db.RegisterMessage(IfaceMeta)
_sym_db.RegisterMessage(IfaceMeta.TraitLabelMappingEntry)

ResourceMeta = _reflection.GeneratedProtocolMessageType('ResourceMeta', (_message.Message,), dict(
  DESCRIPTOR = _RESOURCEMETA,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.ResourceMeta)
  ))
_sym_db.RegisterMessage(ResourceMeta)

TraitId = _reflection.GeneratedProtocolMessageType('TraitId', (_message.Message,), dict(
  DESCRIPTOR = _TRAITID,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitId)
  ))
_sym_db.RegisterMessage(TraitId)

Patch = _reflection.GeneratedProtocolMessageType('Patch', (_message.Message,), dict(
  DESCRIPTOR = _PATCH,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.Patch)
  ))
_sym_db.RegisterMessage(Patch)

TraitState = _reflection.GeneratedProtocolMessageType('TraitState', (_message.Message,), dict(
  DESCRIPTOR = _TRAITSTATE,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitState)
  ))
_sym_db.RegisterMessage(TraitState)

TraitTypeObserveParams = _reflection.GeneratedProtocolMessageType('TraitTypeObserveParams', (_message.Message,), dict(
  DESCRIPTOR = _TRAITTYPEOBSERVEPARAMS,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitTypeObserveParams)
  ))
_sym_db.RegisterMessage(TraitTypeObserveParams)

TraitInstanceObserveParams = _reflection.GeneratedProtocolMessageType('TraitInstanceObserveParams', (_message.Message,), dict(
  DESCRIPTOR = _TRAITINSTANCEOBSERVEPARAMS,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitInstanceObserveParams)
  ))
_sym_db.RegisterMessage(TraitInstanceObserveParams)

ObserveRequest = _reflection.GeneratedProtocolMessageType('ObserveRequest', (_message.Message,), dict(
  DESCRIPTOR = _OBSERVEREQUEST,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.ObserveRequest)
  ))
_sym_db.RegisterMessage(ObserveRequest)

TraitOperationList = _reflection.GeneratedProtocolMessageType('TraitOperationList', (_message.Message,), dict(
  DESCRIPTOR = _TRAITOPERATIONLIST,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.TraitOperationList)
  ))
_sym_db.RegisterMessage(TraitOperationList)

ObserveResponse = _reflection.GeneratedProtocolMessageType('ObserveResponse', (_message.Message,), dict(
  DESCRIPTOR = _OBSERVERESPONSE,
  __module__ = 'nestlabs.gateway.v2.gateway_api_pb2'
  # @@protoc_insertion_point(class_scope:nestlabs.gateway.v2.ObserveResponse)
  ))
_sym_db.RegisterMessage(ObserveResponse)


DESCRIPTOR.has_options = True
DESCRIPTOR._options = _descriptor._ParseOptions(descriptor_pb2.FileOptions(), _b('\n\027com.nestlabs.gateway.v2P\001\242\002\003PCL'))
_IFACEMETA_TRAITLABELMAPPINGENTRY.has_options = True
_IFACEMETA_TRAITLABELMAPPINGENTRY._options = _descriptor._ParseOptions(descriptor_pb2.MessageOptions(), _b('8\001'))

_GATEWAYSERVICE = _descriptor.ServiceDescriptor(
  name='GatewayService',
  full_name='nestlabs.gateway.v2.GatewayService',
  file=DESCRIPTOR,
  index=0,
  options=None,
  serialized_start=2162,
  serialized_end=2266,
  methods=[
  _descriptor.MethodDescriptor(
    name='Observe',
    full_name='nestlabs.gateway.v2.GatewayService.Observe',
    index=0,
    containing_service=None,
    input_type=_OBSERVEREQUEST,
    output_type=_OBSERVERESPONSE,
    options=None,
  ),
])
_sym_db.RegisterServiceDescriptor(_GATEWAYSERVICE)

DESCRIPTOR.services_by_name['GatewayService'] = _GATEWAYSERVICE

# @@protoc_insertion_point(module_scope)
