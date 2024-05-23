// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/alarms/alarms_list_screen/alarms_list.dart';
import 'package:snoozeless/alarms/edit_device/edit_device_screen.dart';
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/shared/app_bar_button.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class AlarmsListScreen extends StatelessWidget {
  final Device device;

  const AlarmsListScreen({Key? key, required this.device}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: StreamBuilder(
          stream: FirestoreService().streamDevice(device.deviceId),
          builder: (context, snapshot) {
            if (snapshot.hasData) {
              Device device = snapshot.data! as Device;
              return Text(device.deviceName);
            } else {
              return Text(device.deviceName);
            }
          },
        ),
        actions: [
          AppBarButton(
            icon: FontAwesomeIcons.pen,
            method: () {
              Navigator.of(context).push(
                MaterialPageRoute(
                  builder: (BuildContext context) =>
                      EditDeviceScreen(deviceId: device.deviceId),
                ),
              );
            },
          ),
        ],
      ),
      body: AlarmsList(
        deviceId: device.deviceId,
      ),
    );
  }
}
