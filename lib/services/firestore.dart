import 'dart:async';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:rxdart/rxdart.dart';
import 'package:snoozeless/services/auth.dart';
import 'package:snoozeless/services/models.dart';

class FirestoreService {
  final FirebaseFirestore _db = FirebaseFirestore.instance;

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

  Stream<Iterable<Device>> streamDevicesList() {
    return AuthService().userStream.switchMap((user) {
      if (user != null) {
        var ref = _db.collection('users').doc(user.uid).collection('devices');
        return ref.snapshots().map(
            (event) => event.docs.map((doc) => Device.fromJson(doc.data())));
      } else {
        return Stream.fromIterable([]);
      }
    });
  }

  Stream<Iterable<Alarm>> streamAlarmsList(String deviceId) {
    return AuthService().userStream.switchMap((user) {
      if (user != null) {
        var ref = _db
            .collection('users')
            .doc(user.uid)
            .collection('devices')
            .doc(deviceId)
            .collection('alarms');
        return ref.snapshots().map(
            (event) => event.docs.map((doc) => Alarm.fromJson(doc.data())));
      } else {
        return Stream.fromIterable([]);
      }
    });
  }

  // Future<void> newDevice(Device device) {
  //   var user = AuthService().user!;
  //   var ref = _db.collection('')
  // }

  Future<void> updateDevice(Device device) {
    var user = AuthService().user!;
    var ref = _db
        .collection('users')
        .doc(user.uid)
        .collection('devices')
        .doc(device.deviceId);

    var data = {
      'deviceName': device.deviceName,
      'timeZoneAdjustment': device.timeZoneAdjustment,
    };

    return ref.update(data /*, SetOptions(merge: true)*/);
  }
}
