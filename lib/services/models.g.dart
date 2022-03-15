// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'models.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

Alarm _$AlarmFromJson(Map<String, dynamic> json) => Alarm(
      alarmId: json['alarmId'] as String? ?? '',
      deviceId: json['deviceId'] as String? ?? '',
      alarmName: json['alarmName'] as String? ?? '',
      countdownDuration: json['countdownDuration'] as int? ?? 0,
      destination: json['destination'] as String? ?? '',
      wakeupTime: json['wakeupTime'] as int? ?? 0,
    );

Map<String, dynamic> _$AlarmToJson(Alarm instance) => <String, dynamic>{
      'alarmId': instance.alarmId,
      'deviceId': instance.deviceId,
      'alarmName': instance.alarmName,
      'countdownDuration': instance.countdownDuration,
      'destination': instance.destination,
      'wakeupTime': instance.wakeupTime,
    };

Device _$DeviceFromJson(Map<String, dynamic> json) => Device(
      deviceId: json['deviceId'] as String? ?? '',
      deviceName: json['deviceName'] as String? ?? 'Device Name',
      timeZoneAdjustment:
          (json['timeZoneAdjustment'] as num?)?.toDouble() ?? 0.0,
      alarms: (json['alarms'] as List<dynamic>?)
              ?.map((e) => Alarm.fromJson(e as Map<String, dynamic>))
              .toList() ??
          const [],
    );

Map<String, dynamic> _$DeviceToJson(Device instance) => <String, dynamic>{
      'deviceId': instance.deviceId,
      'deviceName': instance.deviceName,
      'timeZoneAdjustment': instance.timeZoneAdjustment,
      'alarms': instance.alarms,
    };

User _$UserFromJson(Map<String, dynamic> json) => User(
      email: json['email'] as String? ?? '',
      firstName: json['firstName'] as String? ?? '',
      lastName: json['lastName'] as String? ?? '',
      userId: json['userId'] as String? ?? '',
      devices: (json['devices'] as List<dynamic>?)
              ?.map((e) => Device.fromJson(e as Map<String, dynamic>))
              .toList() ??
          const [],
    );

Map<String, dynamic> _$UserToJson(User instance) => <String, dynamic>{
      'userId': instance.userId,
      'email': instance.email,
      'firstName': instance.firstName,
      'lastName': instance.lastName,
      'devices': instance.devices,
    };
