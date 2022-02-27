// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/devices_screen/new_device/new_device_form.dart';

class NewDeviceScreen extends StatelessWidget {
  const NewDeviceScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('New Device'),
      ),
      body: NewDeviceForm(),
    );
  }
}
