import 'package:json_annotation/json_annotation.dart';
part 'models.g.dart';

@JsonSerializable()
class Alarm {
  final String alarmId;
  final String alarmName;
  final int countdownDuration;
  final String destination;
  final int startTime;

  Alarm({
    this.alarmId = '',
    this.alarmName = '',
    this.countdownDuration = 0,
    this.destination = '',
    this.startTime = 0,
  });

  factory Alarm.fromJson(Map<String, dynamic> json) => _$AlarmFromJson(json);
  Map<String, dynamic> toJson() => _$AlarmToJson(this);
}

@JsonSerializable()
class Device {
  final String deviceId;
  String deviceName;
  int timeZoneAdjustment;
  final List<Alarm> alarms;

  Device({
    this.deviceId = '',
    this.deviceName = 'Device Name',
    this.timeZoneAdjustment = 0,
    this.alarms = const [],
  });

  factory Device.fromJson(Map<String, dynamic> json) => _$DeviceFromJson(json);
  Map<String, dynamic> toJson() => _$DeviceToJson(this);
}

@JsonSerializable()
class User {
  final String email;
  final String firstName;
  final String lastName;
  final String userId;
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
