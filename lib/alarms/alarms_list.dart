import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/alarms/alarms_list_item.dart';
import 'package:snoozeless/alarms/edit_device.dart';

class AlarmsList extends StatelessWidget {
  final String deviceId;
  final String deviceName;

  const AlarmsList({Key? key, required this.deviceId, required this.deviceName})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    // Device stream
    return StreamBuilder<Device>(
      stream: FirestoreService().streamDevice(deviceId),
      builder: (context, deviceSnapshot) {
        // Alarm list stream
        return StreamBuilder<Iterable<Alarm>>(
          stream: FirestoreService().streamAlarmsList(deviceId),
          builder: (context, alarmsSnapshot) {
            if (alarmsSnapshot.connectionState == ConnectionState.waiting) {
              return Scaffold(
                appBar: AppBar(
                  title: Text(deviceName),
                  actions: [
                    IconButton(
                      icon: const Icon(
                        FontAwesomeIcons.pen,
                        size: 20,
                      ),
                      alignment: Alignment.center,
                      onPressed: () {
                        Navigator.of(context).push(
                          MaterialPageRoute(
                            builder: (BuildContext context) =>
                                DeviceEditScreen(deviceId: deviceId),
                          ),
                        );
                      },
                    ),
                  ],
                ),
                body: const Center(/* Insert loading action */),
              );
            } else if (alarmsSnapshot.hasError) {
              return const Text('Error in alarms.dart');
            } else if (alarmsSnapshot.hasData) {
              var device = deviceSnapshot.data!;
              var alarms = alarmsSnapshot.data!;

              return Scaffold(
                appBar: AppBar(
                  title: Text(device.deviceName),
                  actions: [
                    IconButton(
                      icon: const Icon(
                        FontAwesomeIcons.pen,
                        size: 20,
                      ),
                      alignment: Alignment.center,
                      onPressed: () {
                        Navigator.of(context).push(
                          MaterialPageRoute(
                            builder: (BuildContext context) =>
                                DeviceEditScreen(deviceId: deviceId),
                          ),
                        );
                      },
                    ),
                  ],
                ),
                body: ListView(
                  padding: const EdgeInsets.all(20),
                  children: alarms
                      .map((alarm) => AlarmListItem(alarm: alarm))
                      .toList(),
                ),
              );
            } else {
              return const Text('No alarms found. Check database.');
            }
          },
        );
      },
    );
  }
}
