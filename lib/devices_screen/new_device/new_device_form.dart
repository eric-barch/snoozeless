// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/shared/time_zones_map.dart';
import 'package:snoozeless/shared/edit_device_bottom_button.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class NewDeviceForm extends StatefulWidget {
  const NewDeviceForm({Key? key}) : super(key: key);

  @override
  _NewDeviceFormState createState() => _NewDeviceFormState();
}

class _NewDeviceFormState extends State<NewDeviceForm> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  @override
  Widget build(BuildContext context) {
    var localDevice = Device();

    return Form(
      key: _formKey,
      child: Container(
        padding: const EdgeInsets.only(
          left: 20,
          right: 20,
          top: 25,
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            const Text('Device Name'),
            Container(
              padding: const EdgeInsets.only(bottom: 45),
              child: TextFormField(
                initialValue: 'New Alarm Clock',
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Device Name cannot be blank.';
                  } else {
                    localDevice.deviceName = value;
                    return null;
                  }
                },
              ),
            ),
            const Text('Time Zone'),
            Container(
              padding: const EdgeInsets.only(bottom: 45),
              child: DropdownButtonFormField(
                value: timeZonesMap.keys.firstWhere(
                    (key) =>
                        // TODO: Default this to user's current location.
                        timeZonesMap[key] == -5,
                    orElse: () => null),
                items: timeZonesMap.keys
                    .map((key) => key.toString())
                    .toList()
                    .map<DropdownMenuItem<String>>((String value) {
                  return DropdownMenuItem<String>(
                    value: value,
                    child: Text(value),
                  );
                }).toList(),
                validator: (value) {
                  localDevice.timeZoneAdjustment = timeZonesMap[value];
                },
                onChanged: (value) {},
              ),
            ),
            Row(
              children: [
                EditDeviceBottomButton(
                  color: Colors.green,
                  icon: FontAwesomeIcons.check,
                  text: 'Save',
                  method: () {
                    if (_formKey.currentState!.validate()) {
                      FirestoreService().addDevice(
                        localDevice,
                      );
                      Navigator.of(context).pop();
                    }
                  },
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
