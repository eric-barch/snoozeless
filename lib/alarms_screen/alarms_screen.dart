// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/alarms_screen/alarms_list.dart';
import 'package:snoozeless/alarms_screen/edit_device/edit_device_screen.dart';
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/shared/app_bar_button.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class AlarmsScreen extends StatelessWidget {
  final String deviceId;
  final String deviceName;

  const AlarmsScreen(
      {Key? key, required this.deviceId, required this.deviceName})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: StreamBuilder(
          stream: FirestoreService().streamDevice(deviceId),
          builder: (context, snapshot) {
            if (snapshot.hasData) {
              Device device = snapshot.data! as Device;
              return Text(device.deviceName);
            } else {
              return Text(deviceName);
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
                      EditDeviceScreen(deviceId: deviceId),
                ),
              );
            },
          ),
        ],
      ),
      body: AlarmsList(
        deviceId: deviceId,
      ),
    );
  }
}
