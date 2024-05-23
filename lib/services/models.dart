// TODO: Which of these variables should be final?

import 'package:flutter/material.dart';
import 'package:json_annotation/json_annotation.dart';
part 'models.g.dart';

@JsonSerializable()
class Alarm {
  final String alarmId;
  final String deviceId;
  String alarmName;
  int countdownDuration;
  String destination;
  int wakeupTime;

  Alarm({
    this.alarmId = '',
    this.deviceId = '',
    this.alarmName = '',
    this.countdownDuration = 0,
    this.destination = '',
    this.wakeupTime = 0,
  });

  TimeOfDay wakeupTimeOfDay() {
    int hour = wakeupTime ~/ 3600;
    int minute = (wakeupTime % 3600) ~/ 60;
    var wakeupTimeOfDay = TimeOfDay(hour: hour, minute: minute);
    return wakeupTimeOfDay;
  }

  void setWakeupTime(TimeOfDay timeOfDay) {
    int hour = timeOfDay.hour;
    int minute = timeOfDay.minute;
    wakeupTime = (hour * 3600) + (minute * 60);
  }

  factory Alarm.fromJson(Map<String, dynamic> json) => _$AlarmFromJson(json);
  Map<String, dynamic> toJson() => _$AlarmToJson(this);
}

@JsonSerializable()
class Device {
  String deviceId;
  String deviceName;
  // TODO: Change to double and rebuild models.g.dart. Also determine which of these need to be final.
  double timeZoneAdjustment;
  List<Alarm> alarms;

  Device({
    this.deviceId = '',
    this.deviceName = 'Device Name',
    this.timeZoneAdjustment = 0.0,
    this.alarms = const [],
  });

  factory Device.fromJson(Map<String, dynamic> json) => _$DeviceFromJson(json);
  Map<String, dynamic> toJson() => _$DeviceToJson(this);
}

@JsonSerializable()
class User {
  final String userId;
  String email;
  String firstName;
  String lastName;
  final List<Device> devices;

  User({
    this.email = '',
    this.firstName = '',
    this.lastName = '',
    this.userId = '',
    this.devices = const [],
  });

  factory User.fromJson(Map<String, dynamic> json) => _$UserFromJson(json);
  Map<String, dynamic> toJson() => _$UserToJson(this);
}
