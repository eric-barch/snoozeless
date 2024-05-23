import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:rxdart/rxdart.dart';
import 'package:snoozeless/services/auth.dart';
import 'package:snoozeless/services/models.dart';

class FirestoreService {
  final FirebaseFirestore _db = FirebaseFirestore.instance;

  Stream<Iterable<Device>> streamDevicesList() {
    return AuthService().userStream.switchMap((user) {
      if (user != null) {
        var ref = _db
            .collection('users')
            .doc(user.uid)
            .collection('devices')
            .orderBy('createdAt');
        return ref.snapshots().map(
            (event) => event.docs.map((doc) => Device.fromJson(doc.data())));
      } else {
        return Stream.fromIterable([]);
      }
    });
  }

  Stream<Device> streamDevice(String deviceId) {
    return AuthService().userStream.switchMap((user) {
      if (user != null) {
        var ref = _db
            .collection('users')
            .doc(user.uid)
            .collection('devices')
            .doc(deviceId);
        return ref.snapshots().map((doc) => Device.fromJson(doc.data()!));
      } else {
        return Stream.fromIterable([Device()]);
      }
    });
  }

  Future<Device> getDevice(String deviceId) async {
    var user = AuthService().user;
    var uid = user != null ? user.uid : '';
    var ref =
        _db.collection('users').doc(uid).collection('devices').doc(deviceId);
    var snapshot = await ref.get();
    var device = Device.fromJson(snapshot.data()!);
    return device;
  }

  Future<void> updateDevice(Device serverDevice, Device localDevice) {
    var user = AuthService().user!;
    var ref = _db
        .collection('users')
        .doc(user.uid)
        .collection('devices')
        .doc(serverDevice.deviceId);

    var data = {
      'deviceName': localDevice.deviceName,
      'timeZoneAdjustment': localDevice.timeZoneAdjustment,
    };

    return ref.update(data);
  }

  Future<void> deleteDevice(String deviceId) async {
    var user = AuthService().user!;
    var ref = _db
        .collection('users')
        .doc(user.uid)
        .collection('devices')
        .doc(deviceId);
    return ref.delete();
  }

  Future<void> addDevice(Device localDevice) async {
    // TODO: Can these database writes be done in one step?
    var user = AuthService().user!;
    var collectionRef =
        _db.collection('users').doc(user.uid).collection('devices');

    var data = {
      'deviceName': localDevice.deviceName,
      'timeZoneAdjustment': localDevice.timeZoneAdjustment,
      'createdAt': Timestamp.now(),
    };

    DocumentReference docRef = await collectionRef.add(data);

    data = {
      'deviceId': docRef.id,
    };

    return collectionRef.doc(docRef.id).update(data);
  }

  Stream<Iterable<Alarm>> streamAlarmsList(String deviceId) {
    return AuthService().userStream.switchMap((user) {
      if (user != null) {
        var ref = _db
            .collection('users')
            .doc(user.uid)
            .collection('devices')
            .doc(deviceId)
            .collection('alarms')
            .orderBy('createdAt');
        return ref.snapshots().map(
            (event) => event.docs.map((doc) => Alarm.fromJson(doc.data())));
      } else {
        return Stream.fromIterable([]);
      }
    });
  }

  Future<Alarm> getAlarm(Alarm localAlarm) async {
    var user = AuthService().user;
    var uid = user != null ? user.uid : '';
    var ref = _db
        .collection('users')
        .doc(uid)
        .collection('devices')
        .doc(localAlarm.deviceId)
        .collection('alarms')
        .doc(localAlarm.alarmId);
    var snapshot = await ref.get();
    var serverAlarm = Alarm.fromJson(snapshot.data()!);
    return serverAlarm;
  }

  Future<void> updateAlarm(Alarm initialAlarm, Alarm modifiedAlarm) {
    var user = AuthService().user!;
    var ref = _db
        .collection('users')
        .doc(user.uid)
        .collection('devices')
        .doc(initialAlarm.deviceId)
        .collection('alarms')
        .doc(initialAlarm.alarmId);

    var data = {
      'alarmName': modifiedAlarm.alarmName,
      'wakeupTime': modifiedAlarm.wakeupTime,
    };

    return ref.update(data);
  }
}
