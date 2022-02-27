// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/alarms_screen/edit_device/edit_device_form.dart';

class EditDeviceScreen extends StatelessWidget {
  final String deviceId;

  const EditDeviceScreen({Key? key, required this.deviceId}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Edit Device'),
      ),
      body: EditDeviceForm(deviceId: deviceId),
    );
  }
}
